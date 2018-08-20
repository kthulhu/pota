#include <ai.h>
#include "lentil_bruteforce.h"
#include "lens.h"
#include <cmath>
#include "../../polynomial-optics/src/raytrace.h"
#include "../../polynomial-optics/src/lenssystem.h"


AI_CAMERA_NODE_EXPORT_METHODS(lentilbruteforceMethods)


enum
{
  p_lensID,
  p_target_focal_length,
  p_unitModel,
  p_lensModel,
  p_sensor_width,
  p_wavelength,
  p_dof,
  p_fstop,
  p_focal_distance,
  p_extra_sensor_shift,
  p_vignetting_retries,
  p_aperture_blades,
  p_backward_samples,
  p_minimum_rgb,
  p_bokeh_exr_path,
  p_proper_ray_derivatives
};


// to switch between lens models in interface dropdown
// this will need to be automatically filled somehow
static const char* LensModelNames[] =
{
  #include "auto_generated_lens_includes/pota_cpp_lenses.h"
  NULL
};

// to switch between units in interface dropdown
static const char* UnitModelNames[] =
{
  "mm",
  "cm",
  "dm",
  "m",
  NULL
};



node_parameters
{
  AiParameterInt("lensID", 1);
  AiParameterInt("target_focal_length", 100);

  AiParameterEnum("unitModel", cm, UnitModelNames);
  AiParameterEnum("lensModel", angenieux_double_gauss_1953_100mm, LensModelNames); // what to do here..? Can i not specify one?
  AiParameterFlt("sensor_width", 36.0); // 35mm film
  AiParameterFlt("wavelength", 550.0); // wavelength in nm
  AiParameterBool("dof", true);
  AiParameterFlt("fstop", 0.0);
  AiParameterFlt("focal_distance", 150.0); // in cm to be consistent with arnold core
  AiParameterFlt("extra_sensor_shift", 0.0);
  AiParameterInt("vignetting_retries", 15);
  AiParameterInt("aperture_blades", 0);
  AiParameterInt("backward_samples", 3);
  AiParameterFlt("minimum_rgb", 3.0f);
  AiParameterStr("bokeh_exr_path", "");
  AiParameterBool("proper_ray_derivatives", true);
}


node_initialize
{
  AiCameraInitialize(node);
  AiNodeSetLocalData(node, new MyCameraData());
}


node_update
{   
  MyCameraData* camera_data = (MyCameraData*)AiNodeGetLocalData(node);

  camera_data->lensid = AiNodeGetInt(node, "lensID");
  camera_data->target_focal_length = AiNodeGetInt(node, "target_focal_length");

  camera_data->sensor_width = AiNodeGetFlt(node, "sensor_width");
  camera_data->fstop = AiNodeGetFlt(node, "fstop");
  camera_data->focal_distance = AiNodeGetFlt(node, "focal_distance") * 10.0f;
  camera_data->lensModel = (LensModel) AiNodeGetInt(node, "lensModel");
  camera_data->unitModel = (UnitModel) AiNodeGetInt(node, "unitModel");
  camera_data->aperture_blades = AiNodeGetInt(node, "aperture_blades");
  camera_data->dof = AiNodeGetBool(node, "dof");
  camera_data->vignetting_retries = AiNodeGetInt(node, "vignetting_retries");
  camera_data->backward_samples = AiNodeGetInt(node, "backward_samples");
  camera_data->minimum_rgb = AiNodeGetFlt(node, "minimum_rgb");
  camera_data->bokeh_exr_path = AiNodeGetStr(node, "bokeh_exr_path");
  camera_data->proper_ray_derivatives = AiNodeGetBool(node, "proper_ray_derivatives");
  camera_data->sensor_shift = 0.0;

  // convert to cm
  switch (camera_data->unitModel){
    case mm:
    {
      camera_data->focal_distance *= 0.1f;
    } break;
    case dm:
    {
      camera_data->focal_distance *= 10.0f;
    } break;
    case m:
    {
      camera_data->focal_distance *= 100.0f;
    }
  }


  AiMsgInfo("");


  // read lens database
  std::string lens_database_path = std::getenv("LENTIL_PATH");
  lens_database_path += "/database/lenses.json";
  std::ifstream in_json(lens_database_path.c_str());
  json lens_database = json::parse(in_json);
  camera_data->lenses_cnt = lens_configuration(camera_data->lenses, camera_data->lensID, camera_data->target_focal_length);
  camera_data->lens_length = 0.0f;
  float zoom = 0.0f;
  for(int i=0;i<camera_data->lenses_cnt;i++) camera_data->lens_length += lens_get_thickness(camera_data->lenses+i, zoom);
  camera_data->lens_pupil_dist = lens_get_thickness(camera_data->lenses + camera_data->lenses_cnt-1, zoom);
  camera_data->lens_pupil_rad  = camera_data->lenses[camera_data->lenses_cnt-1].housing_radius;
  camera_data->aperture_rad = lens_get_aperture_radius(camera_data->lenses, camera_data->lenses_cnt);



  camera_data->lambda = AiNodeGetFlt(node, "wavelength") * 0.001;
  AiMsgInfo("[POTA] wavelength: %f", camera_data->lambda);

  //camera_data->max_fstop = camera_data->lens_focal_length / (camera_data->lens_aperture_housing_radius * 2.0f);
  //AiMsgInfo("[POTA] lens wide open f-stop: %f", camera_data->max_fstop);

  //if (camera_data->fstop == 0.0f) camera_data->aperture_radius = camera_data->lens_aperture_housing_radius;
  //else camera_data->aperture_radius = fminf(camera_data->lens_aperture_housing_radius, camera_data->lens_focal_length / (2.0f * camera_data->fstop));
  camera_data->aperture_radius = camera_data->aperture_rad;
  //AiMsgInfo("[POTA] full aperture radius: %f", camera_data->lens_aperture_housing_radius);
  AiMsgInfo("[POTA] fstop-calculated aperture radius: %f", camera_data->aperture_radius);
  AiMsgInfo("[POTA] --------------------------------------");


  AiMsgInfo("[POTA] focus distance: %f", camera_data->focal_distance);



  // logartihmic focus search
  float best_sensor_shift = 0.0f;
  //float closest_distance = AI_BIG;
  //logarithmic_focus_search(camera_data->focal_distance, best_sensor_shift, closest_distance, camera_data);
  //AiMsgInfo("[POTA] sensor_shift using logarithmic search: %f", best_sensor_shift);
  camera_data->sensor_shift = best_sensor_shift + AiNodeGetFlt(node, "extra_sensor_shift");


  // logarithmic infinity focus search
  //float best_sensor_shift_infinity = 0.0f;
  //float closest_distance_infinity = AI_BIG;
  //logarithmic_focus_search(AI_BIG, best_sensor_shift_infinity, closest_distance_infinity, camera_data);
  //AiMsgInfo("[POTA] sensor_shift [logarithmic forward tracing] to focus at infinity: %f", best_sensor_shift_infinity);
      
  // bidirectional parallel infinity focus search
  //float infinity_focus_parallel_light_tracing = camera_set_focus_infinity(camera_data);
  //AiMsgInfo("[POTA] sensor_shift [parallel backwards light tracing] to focus at infinity: %f", infinity_focus_parallel_light_tracing);


  // double check where y=0 intersection point is, should be the same as focus distance
  //if(!trace_ray_focus_check(camera_data->sensor_shift, camera_data)){
  //  AiMsgWarning("[POTA] focus check failed. Either the lens system is not correct, or the sensor is placed at a wrong distance.");
  //}


  //camera_data->tan_fov = tanf(camera_data->lens_field_of_view / 2.0f);


  AiMsgInfo("");
  AiCameraUpdate(node, false);
}


node_finish
{

  MyCameraData* camera_data = (MyCameraData*)AiNodeGetLocalData(node);
  delete camera_data;
}


camera_create_ray
{
  MyCameraData* camera_data = (MyCameraData*)AiNodeGetLocalData(node);

  const float lambda = 0.5f;
  float in[5] = {0.0f};
  float out[5] = {0.0f};

  // set cam_pos and cam_dir somehow.
  Eigen::Vector2d unit_disk(0.0f, 0.0f);
  concentric_disk_sample(input.lensx, input.lensy, unit_disk, false);

  
  in[0] = input.sx * (camera_data->sensor_width * 0.5f); 
  in[1] = input.sy * (camera_data->sensor_width * 0.5f); 
  in[2] = camera_data->sensor_shift;

  // these are probably wrong
  out[0] = (unit_disk[0] * camera_data->aperture_rad) - in[0];
  out[1] = (unit_disk[1] * camera_data->aperture_rad) - in[1];
  out[2] = 0.0 - in[2];

  in[4] = out[4] = lambda;



  // evaluate returns in sphere/cyl space? either avoid or convert back
  evaluate(camera_data->lenses, camera_data->lenses_cnt, 0.0, in, out, aspheric);

  // needs to be updated
  // eigen->arnold
  for (int i = 0; i<3; i++){
    output.origin[i] = origin(i);
    output.dir[i] = direction(i);
    output.weight[i] = weight(i);
  }
}


camera_reverse_ray
{
  return false;
}


node_loader
{
  if (i != 0) return false;
  node->methods = lentilbruteforceMethods;
  node->output_type = AI_TYPE_UNDEFINED;
  node->name = "lentilbruteforce";
  node->node_type = AI_NODE_CAMERA;
  strcpy(node->version, AI_VERSION);
  return true;
}