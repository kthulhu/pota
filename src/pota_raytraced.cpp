#include <ai.h>
#include "pota.h"
#include "lens.h"
#include <cmath>
#include <string>
#include "../../polynomial-optics/src/lenssystem.h"
#include "../../polynomial-optics/src/raytrace.h"


AI_CAMERA_NODE_EXPORT_METHODS(pota_raytracedMethods)

struct CameraRaytraced
{
  std::string id;
	int lenses_cnt;
  lens_element_t lenses[50];
  float p_dist;
  float p_rad;
  float zoom;
  float lens_focal_length;
} camera_rt;


enum
{
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
  AiNodeSetLocalData(node, new Camera());
  AiNodeSetLocalData(node, new CameraRaytraced());
}


node_update
{   
  Camera* camera = (Camera*)AiNodeGetLocalData(node);
  CameraRaytraced* camera_rt = (CameraRaytraced*)AiNodeGetLocalData(node);

  camera_rt->lens_focal_length = 100;
  camera_rt->id = "0001";
  camera_rt->zoom = 0.0f;
  camera_rt->lenses_cnt = lens_configuration(camera_rt->lenses, camera_rt->id.c_str(), camera_rt->lens_focal_length);
  camera_rt->p_dist = lens_get_thickness(camera_rt->lenses + camera_rt->lenses_cnt-1, camera_rt->zoom);
  camera_rt->p_rad = camera_rt->lenses[camera_rt->lenses_cnt-1].housing_radius;

  camera->sensor_width = AiNodeGetFlt(node, "sensor_width");
  camera->input_fstop = AiNodeGetFlt(node, "fstop");
  camera->focal_distance = AiNodeGetFlt(node, "focal_distance") * 10.0f;
  camera->lensModel = (LensModel) AiNodeGetInt(node, "lensModel");
  camera->unitModel = (UnitModel) AiNodeGetInt(node, "unitModel");
  camera->aperture_blades = AiNodeGetInt(node, "aperture_blades");
  camera->dof = AiNodeGetBool(node, "dof");
  camera->vignetting_retries = AiNodeGetInt(node, "vignetting_retries");
  camera->backward_samples = AiNodeGetInt(node, "backward_samples");
  camera->minimum_rgb = AiNodeGetFlt(node, "minimum_rgb");
  camera->bokeh_exr_path = AiNodeGetStr(node, "bokeh_exr_path");
  camera->proper_ray_derivatives = AiNodeGetBool(node, "proper_ray_derivatives");
  camera->sensor_shift = 0.0f;

  // convert to cm
  switch (camera->unitModel){
    case mm:
    {
      camera->focal_distance *= 0.1f;
    } break;
    case dm:
    {
      camera->focal_distance *= 10.0f;
    } break;
    case m:
    {
      camera->focal_distance *= 100.0f;
    }
  }


  AiMsgInfo("");


/*
  //load_lens_constants(camera);
  AiMsgInfo("[POTA] ----------  LENS CONSTANTS  -----------");
  AiMsgInfo("[POTA] Lens Name: %s", camera->lens_name);
  AiMsgInfo("[POTA] Lens F-Stop: %f", camera->lens_fstop);
#ifdef DEBUG_LOG
  AiMsgInfo("[POTA] lens_outer_pupil_radius: %f", camera->lens_outer_pupil_radius);
  AiMsgInfo("[POTA] lens_inner_pupil_radius: %f", camera->lens_inner_pupil_radius);
  AiMsgInfo("[POTA] lens_length: %f", camera->lens_length);
  AiMsgInfo("[POTA] lens_back_focal_length: %f", camera->lens_back_focal_length);
  AiMsgInfo("[POTA] lens_effective_focal_length: %f", camera->lens_effective_focal_length);
  AiMsgInfo("[POTA] lens_aperture_pos: %f", camera->lens_aperture_pos);
  AiMsgInfo("[POTA] lens_aperture_housing_radius: %f", camera->lens_aperture_housing_radius);
  AiMsgInfo("[POTA] lens_outer_pupil_curvature_radius: %f", camera->lens_outer_pupil_curvature_radius);
  AiMsgInfo("[POTA] lens_outer_pupil_geometry: %s", camera->lens_outer_pupil_geometry.c_str());
  AiMsgInfo("[POTA] lens_field_of_view: %f", camera->lens_field_of_view);
#endif
  AiMsgInfo("[POTA] --------------------------------------");
*/

  camera->lambda = AiNodeGetFlt(node, "wavelength") * 0.001;
  AiMsgInfo("[POTA] wavelength: %f", camera->lambda);
/*
  //camera->max_fstop = camera->lens_effective_focal_length / (camera->lens_aperture_housing_radius * 2.0f);
  AiMsgInfo("[POTA] lens wide open f-stop: %f", camera->lens_fstop);

  if (camera->input_fstop == 0.0f) camera->aperture_radius = camera->lens_aperture_housing_radius;
  // how do i get to proper aperture radius..?
  else camera->aperture_radius = fminf(camera->lens_aperture_housing_radius, camera->lens_effective_focal_length / (2.0f * camera->input_fstop));

  AiMsgInfo("[POTA] full aperture radius: %f", camera->lens_aperture_housing_radius);
  AiMsgInfo("[POTA] fstop-calculated aperture radius: %f", camera->aperture_radius);
  AiMsgInfo("[POTA] --------------------------------------");
*/

  AiMsgInfo("[POTA] focus distance: %f", camera->focal_distance);

  /* was already commented out
  AiMsgInfo("[POTA] calculating sensor shift at focus distance:");
  camera->sensor_shift = camera_set_focus(camera->focal_distance, camera);
  AiMsgInfo("[POTA] sensor_shift to focus at %f: %f", camera->focal_distance, camera->sensor_shift);
  */
/*
  // logartihmic focus search
  float best_sensor_shift = 0.0f;
  float closest_distance = AI_BIG;
  logarithmic_focus_search(camera->focal_distance, best_sensor_shift, closest_distance, camera);
  AiMsgInfo("[POTA] sensor_shift using logarithmic search: %f", best_sensor_shift);
  camera->sensor_shift = best_sensor_shift + AiNodeGetFlt(node, "extra_sensor_shift");
*/
  /* was already commented out
  // average guesses infinity focus search
  float infinity_focus_sensor_shift = camera_set_focus(AI_BIG, camera);
  AiMsgInfo("[POTA] sensor_shift [average guesses backwards light tracing] to focus at infinity: %f", infinity_focus_sensor_shift);
  */
/*
  // logarithmic infinity focus search
  float best_sensor_shift_infinity = 0.0f;
  float closest_distance_infinity = AI_BIG;
  logarithmic_focus_search(AI_BIG, best_sensor_shift_infinity, closest_distance_infinity, camera);
  AiMsgInfo("[POTA] sensor_shift [logarithmic forward tracing] to focus at infinity: %f", best_sensor_shift_infinity);
      
  // bidirectional parallel infinity focus search
  float infinity_focus_parallel_light_tracing = camera_set_focus_infinity(camera);
  AiMsgInfo("[POTA] sensor_shift [parallel backwards light tracing] to focus at infinity: %f", infinity_focus_parallel_light_tracing);


  // double check where y=0 intersection point is, should be the same as focus distance
  if(!trace_ray_focus_check(camera->sensor_shift, camera)){
    AiMsgWarning("[POTA] focus check failed. Either the lens system is not correct, or the sensor is placed at a wrong distance.");
  }


  camera->tan_fov = tanf(camera->lens_field_of_view / 2.0f);
*/

  /*
  need to implement:
    - sensorshift (autofocus)

  */



  AiMsgInfo("");
  AiCameraUpdate(node, false);
}


node_finish
{
  Camera* camera = (Camera*)AiNodeGetLocalData(node);
  CameraRaytraced* camera_rt = (CameraRaytraced*)AiNodeGetLocalData(node);

  delete camera;
  delete camera_rt;
}


camera_create_ray
{
  Camera* camera = (Camera*)AiNodeGetLocalData(node);
  CameraRaytraced* camera_rt = (CameraRaytraced*)AiNodeGetLocalData(node);

  int tries = 0;
  bool ray_success = false;
  camera->vignetting_retries = 0;

  float pos[3] = {0.0f};
  float dir[3] = {0.0f};

  while(ray_success == false && tries <= camera->vignetting_retries)
  {
    // reset pos&dir, is this needed?
    for(int i = 0; i<3; i++){
      pos[i] = 0.0;
      dir[i] = 0.0;
    }

    // need to implement the sensor shift
    float ray_in[5] = {0.0};
    ray_in[0] = input.sx * (camera->sensor_width * 0.5f);
    ray_in[1] = input.sy * (camera->sensor_width * 0.5f);
    ray_in[2] = (camera_rt->p_rad/camera_rt->p_dist * (2.0*input.lensx-1.0)) - (ray_in[0] / camera_rt->p_dist);
    ray_in[3] = (camera_rt->p_rad/camera_rt->p_dist * (2.0*input.lensy-1.0)) - (ray_in[1] / camera_rt->p_dist);
    ray_in[4] = camera->lambda;

    float out[5];

    static int aspheric_elements = 1;
    int error = evaluate_for_pos_dir(camera_rt->lenses, camera_rt->lenses_cnt, camera_rt->zoom, ray_in, out, aspheric_elements, pos, dir);
    if(error){
      ++tries;
      continue;
    }

    ray_success = true;
  }

  if (!ray_success){
    output.weight = {0.0, 0.0, 0.0};
    return;
  }
  
  for (int i = 0; i<3; i++){
    output.origin[i] = pos[i];
    output.dir[i] = dir[i];
  }

  // why do i have to divide by 10 only for raytraced model?
  for (int i = 0; i<3; i++){
    output.origin[i] *= 0.1;
    output.dir[i] *= 0.1;
  }

  switch (camera->unitModel){
    case mm:
    {
      output.origin *= -1.0; // reverse rays and convert to cm
      output.dir *= -1.0; //reverse rays and convert to cm
    } break;
    case cm:
    { 
      output.origin *= -0.1; // reverse rays and convert to cm
      output.dir *= -0.1; //reverse rays and convert to cm
    } break;
    case dm:
    {
      output.origin *= -0.01; // reverse rays and convert to cm
      output.dir *= -0.01; //reverse rays and convert to cm
    } break;
    case m:
    {
      output.origin *= -0.001; // reverse rays and convert to cm
      output.dir *= -0.001; //reverse rays and convert to cm
    }
  }

  AiV3Normalize(output.dir);

  // calculate new ray derivatives
  if (tries > 0){
    if (!camera->proper_ray_derivatives){
      for(int i=0; i<3; i++){
        output.dOdy[i] = output.origin[i];
        output.dDdy[i] = output.dir[i];
      }
    }
  }
} 



// approximation using pinhole camera FOV
camera_reverse_ray
{
  /*
  const Camera* camera = (Camera*)AiNodeGetLocalData(node);

  float coeff = 1.0 / AiMax(fabsf(Po.z * camera->tan_fov), 1e-3f);
  Ps.x = Po.x * coeff;
  Ps.y = Po.y * coeff;

  return true;
  */
 return false;
}


node_loader
{
  if (i != 0) return false;
  node->methods = pota_raytracedMethods;
  node->output_type = AI_TYPE_UNDEFINED;
  node->name = "pota_raytraced";
  node->node_type = AI_NODE_CAMERA;
  strcpy(node->version, AI_VERSION);
  return true;
}