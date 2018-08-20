#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include "../../polynomial-optics/src/lenssystem.h"


// enum to switch between lens models in interface dropdown
enum LensModel{
    #include "auto_generated_lens_includes/pota_h_lenses.h"
};


// enum to switch between units in interface dropdown
enum UnitModel{
    mm,
    cm,
    dm,
    m
};


struct MyCameraData
{
	LensModel lensModel;
    UnitModel unitModel;

    const char* lens_name;
    int lensID;
    int target_focal_length;
    float lens_length;
    float lens_aperture_pos;
    int lenses_cnt;
    float lens_pupil_dist;
    float lens_pupil_rad;
    flaot aperture_rad;
    std::string lens_outer_pupil_geometry;


	float sensor_width;
	float fstop;
    float max_fstop;
	float focal_distance;
	float aperture_radius;
	float sensor_shift;
    int vignetting_retries;
	float lambda;
    int aperture_blades;
	bool dof;
    int backward_samples;
    float minimum_rgb;
    AtString bokeh_exr_path;

    bool run_intersection_tests;
    int count;

    bool proper_ray_derivatives;

    float random1;
    float random2;

    //camera_reverse_ray
    float tan_fov;


    lens_element_t lenses[50];
};

extern struct MyCameraData camera_data;