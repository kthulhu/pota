#include <ai.h>
#include <vector>
#include <map>
#include "pota.h"
#include "lens.h"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

AI_SHADER_NODE_EXPORT_METHODS(PotaBokehAOVMtd);


inline void replace_frame_numbering(std::string &original_string){
  std::string substring = "";
  size_t numberofhashes = std::count(original_string.begin(), original_string.end(), '#');
  for (int i = 0; i < numberofhashes; i++) substring.insert(0, "#");

  std::string framestring = std::to_string(static_cast<int>(AiNodeGetFlt(AiUniverseGetOptions(), "frame")));
  framestring.insert(0, substring.length() - framestring.length(), '0');
  original_string.replace(original_string.find(substring), substring.length(), framestring);
}

inline void add_aov_suffix(std::string &original_string, AtString aov_name){
  std::string path = original_string;
  path += "_";
  path += aov_name.c_str();
  original_string = path;
}


struct PotaBokehAOVData
{
  AtString bokeh_aov_name;
  AtString discarded_samples_aov_name;
  int aa_samples;
  int xres;
  int yres;
  int samples;
  std::map<AtString, std::vector<AtRGBA>> image_buffers;
};
 

enum SampleBokehParams
{
   p_bokeh_aov_name,
   p_discarded_samples_aov_name,
};


node_parameters
{
   AiParameterStr("bokeh_aov_name", "");
   AiMetaDataSetBool(nentry, "bokeh_aov_name", "linkable", false);
   AiParameterStr("discarded_samples_aov_name", "");
   AiMetaDataSetBool(nentry, "discarded_samples_aov_name", "linkable", false);
}

 
node_initialize
{
   AiNodeSetLocalData(node, new PotaBokehAOVData());
}

 
node_update
{
  PotaBokehAOVData *bokeh_data = (PotaBokehAOVData*)AiNodeGetLocalData(node);
  //const MyCameraData *camera_data = (MyCameraData*)AiNodeGetLocalData(AiUniverseGetCamera());

  /*
  // register AOVs
  bokeh_data->bokeh_aov_name = AiNodeGetStr(node, "bokeh_aov_name");
  AiAOVRegister(bokeh_data->bokeh_aov_name, AI_TYPE_RGBA, AI_AOV_BLEND_OPACITY);
  bokeh_data->discarded_samples_aov_name = AiNodeGetStr(node, "discarded_samples_aov_name");
  AiAOVRegister(bokeh_data->discarded_samples_aov_name, AI_TYPE_RGBA, AI_AOV_BLEND_OPACITY);
  */

  // get general options
  bokeh_data->aa_samples = AiNodeGetInt(AiUniverseGetOptions(), "AA_samples");
  bokeh_data->xres = AiNodeGetInt(AiUniverseGetOptions(), "xres");
  bokeh_data->yres = AiNodeGetInt(AiUniverseGetOptions(), "yres");


  bokeh_data->image_buffers.clear();
  //bokeh_data->image.reserve(bokeh_data->xres * bokeh_data->yres);


  AtAOVIterator* aov_iterator = AiUniverseGetAOVIterator();
  while (!AiAOVIteratorFinished(aov_iterator))
  {
    const AtAOVEntry* aov_entry = AiAOVIteratorGetNext(aov_iterator);
    // this should in theory be able to get all the active aov's
    AiMsgInfo("aov_entry: %s", aov_entry->name.c_str());

    // register extra aovs, ugly string conversions can be improved
    std::string discarded_aov_name = aov_entry->name.c_str();
    discarded_aov_name += "_discarded";
    std::string bokeh_aov_name = aov_entry->name.c_str();
    bokeh_aov_name += "_bokeh";
    AiAOVRegister(AtString(discarded_aov_name.c_str()), aov_entry->type, aov_entry->blend_mode);
    AiAOVRegister(AtString(bokeh_aov_name.c_str()), aov_entry->type, aov_entry->blend_mode);

    // create image buffers, currently probably creating a lot of them...
    std::vector<AtRGBA> current_buffer;
    current_buffer.reserve(bokeh_data->xres * bokeh_data->yres);
    bokeh_data->image_buffers.insert(std::make_pair(aov_entry->name, current_buffer));
  }
  AiAOVIteratorDestroy(aov_iterator);
}


node_finish
{
  PotaBokehAOVData *bokeh_data = (PotaBokehAOVData*)AiNodeGetLocalData(node);
  const MyCameraData *camera_data = (MyCameraData*)AiNodeGetLocalData(AiUniverseGetCamera());


  AtAOVIterator* aov_iterator = AiUniverseGetAOVIterator();
  while (!AiAOVIteratorFinished(aov_iterator))
  {
    const AtAOVEntry* aov_entry = AiAOVIteratorGetNext(aov_iterator);

    //image_buffers.insert(std::make_pair(aov_entry->name, current_buffer));

    std::vector<float> image(bokeh_data->yres * bokeh_data->xres * 4);
    int offset = -1;
    int pixelnumber = 0;
    int aa_square = bokeh_data->aa_samples * bokeh_data->aa_samples;

    for(auto i = 0; i < bokeh_data->xres * bokeh_data->yres; i++){
      image[++offset] = bokeh_data->image_buffers[aov_entry->name][pixelnumber].r / (float)aa_square;
      image[++offset] = bokeh_data->image_buffers[aov_entry->name][pixelnumber].g / (float)aa_square;
      image[++offset] = bokeh_data->image_buffers[aov_entry->name][pixelnumber].b / (float)aa_square;
      image[++offset] = bokeh_data->image_buffers[aov_entry->name][pixelnumber].a / (float)aa_square;
      ++pixelnumber;
    }

    // replace frame numbering
    std::string original_string = camera_data->bokeh_exr_path.c_str();
    replace_frame_numbering(original_string);
    add_aov_suffix(original_string, aov_entry->name);
    SaveEXR(image.data(), bokeh_data->xres, bokeh_data->yres, 4, 0, original_string.c_str());
    AiMsgWarning("[POTA] Bokeh AOV written to %s", original_string.c_str());  
  }
  AiAOVIteratorDestroy(aov_iterator);


  

  delete bokeh_data;
}


shader_evaluate
{
   PotaBokehAOVData *bokeh_data = (PotaBokehAOVData*)AiNodeGetLocalData(node);
   MyCameraData *camera_data = (MyCameraData*)AiNodeGetLocalData(AiUniverseGetCamera());

   // why does this need to be in shader_evaluate to work? returns 0 in _update_?
   bokeh_data->samples = camera_data->backward_samples * (bokeh_data->aa_samples * bokeh_data->aa_samples);

   const float xres = (float)bokeh_data->xres;
   const float yres = (float)bokeh_data->yres;
   const float frame_aspect_ratio = xres/yres;

   // write AOV only if in use
   if ((sg->Rt & AI_RAY_CAMERA))
   {
      // figure out better way, based on:
      // distance from focus point
      // intensity of sample
      if ((sg->out.RGBA().r > camera_data->minimum_rgb) || (sg->out.RGBA().g > camera_data->minimum_rgb) || (sg->out.RGBA().b > camera_data->minimum_rgb))
      {

        std::map<AtString, AtRGBA> sample_energy_per_aov;
        
        // write discarded samples into separate AOV to be able to difference
        //if (AiAOVEnabled(bokeh_data->discarded_samples_aov_name, AI_TYPE_RGBA)){
        //  AiAOVSetRGBA(sg, bokeh_data->discarded_samples_aov_name, sg->out.RGBA());
        //}
        AtAOVIterator* aov_iterator = AiUniverseGetAOVIterator();
        while (!AiAOVIteratorFinished(aov_iterator))
        {
          const AtAOVEntry* aov_entry = AiAOVIteratorGetNext(aov_iterator);
          if (AiAOVEnabled(aov_entry->name, aov_entry->type)){
            // how do i query the value of an aov? Right now i have to explicityly put the AOVType..
            AtRGBA aov_value;
            // will only read if aov_value type matches the aov type
            if (AiAOVGetRGBA(sg, aov_entry->name, aov_value)){
              std::string tmpstring = aov_entry->name.c_str();
              tmpstring += "_discarded";
              AiAOVSetRGBA(sg, AtString(tmpstring.c_str()), aov_value);

              // calculate value per sample
              AtRGBA sample_energy = aov_value;
              sample_energy.a = 1.0f;
              sample_energy /= static_cast<float>(bokeh_data->samples);
              sample_energy_per_aov[aov_entry->name] = sample_energy;
            }
          }
        }
        


        // convert sample world space position to camera space
        AtMatrix world_to_camera_matrix;
        Eigen::Vector2d sensor_position;

        AiWorldToCameraMatrix(AiUniverseGetCamera(), sg->time, world_to_camera_matrix);
        // improve this, too much copying
        AtVector camera_space_sample_position_tmp = AiM4PointByMatrixMult(world_to_camera_matrix, sg->P);
        Eigen::Vector3d camera_space_sample_position(camera_space_sample_position_tmp.x, camera_space_sample_position_tmp.y, camera_space_sample_position_tmp.z);
         
        for(int count=0; count<bokeh_data->samples; count++)
        {
          if(!trace_backwards( -camera_space_sample_position * 10.0, camera_data->aperture_radius, camera_data->lambda, sensor_position, camera_data->sensor_shift, camera_data))
          {
            continue;
            --count;
          }

          // convert sensor position to pixel position
          Eigen::Vector2d s(sensor_position(0) / (camera_data->sensor_width * 0.5), sensor_position(1) / (camera_data->sensor_width * 0.5) * frame_aspect_ratio);

          const float pixel_x = ((s(0) + 1.0) / 2.0) * xres;
          const float pixel_y = ((-s(1) + 1.0) / 2.0) * yres;

//figure out why sometimes pixel is nan, can't just skip it
          if ((pixel_x > xres) || 
              (pixel_x < 0)    || 
              (pixel_y > yres) || 
              (pixel_y < 0)    || 
              (pixel_x != pixel_x) ||  //nan checking
              (pixel_y != pixel_y)) // nan checking
          {
            continue;
            --count;
          }

          // write sample to image
          int pixelnumber = static_cast<int>(bokeh_data->xres * floor(pixel_y) + floor(pixel_x));
          
          AtAOVIterator* aov_iterator = AiUniverseGetAOVIterator();
          while (!AiAOVIteratorFinished(aov_iterator))
          {
            const AtAOVEntry* aov_entry = AiAOVIteratorGetNext(aov_iterator);
            if (AiAOVEnabled(aov_entry->name, aov_entry->type)){
              bokeh_data->image_buffers[aov_entry->name][pixelnumber] += sample_energy_per_aov[aov_entry->name];
            }
          }
          
        }
      }

    // ideally would be cool to write to an aov but not sure if I can access the different pixels other than
    // the one related to the current sample
    // AtRGBA aov_value = bokeh_data->image[bokeh_data->xres * (sg->y+1, sg->x+1)]; // IS THE +1 CORRECT? 
    // AiAOVSetRGBA(sg, bokeh_data->bokeh_aov_name, aov_value);
  }
}
 
node_loader
{
  if (i != 0) return false;
  node->methods     = PotaBokehAOVMtd;
  node->output_type = AI_TYPE_RGBA;
  node->name        = "potabokehAOV";
  node->node_type   = AI_NODE_SHADER;
  strcpy(node->version, AI_VERSION);
  return true;
}