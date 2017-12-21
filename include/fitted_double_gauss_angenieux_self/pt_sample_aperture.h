float pred_x;
float pred_y;
float pred_dx;
float pred_dy;
float sqr_err = FLT_MAX;
for(int k=0;k<5&&sqr_err > 1e-4f;k++)
{
  const float begin_x = x + dist * dx;
  const float begin_y = y + dist * dy;
  const float begin_dx = dx;
  const float begin_dy = dy;
  __attribute__((unused)) const float begin_lambda = lambda;
  pred_x =  + 26.7119 *begin_dx + 1.09538 *begin_x*begin_lambda + 6.60947 *begin_dx*begin_lambda + -0.0547589 *begin_y*begin_dx*begin_dy + 0.00189755 *lens_ipow(begin_x, 2)*begin_dx + -16.5359 *lens_ipow(begin_dx, 3) + -1.63151 *begin_x*lens_ipow(begin_lambda, 2) + -14.2808 *begin_dx*lens_ipow(begin_dy, 2) + -0.000478074 *begin_x*lens_ipow(begin_y, 2) + -0.000412757 *lens_ipow(begin_x, 3) + -0.184661 *begin_x*lens_ipow(begin_dy, 2) + -4.54335 *begin_dx*lens_ipow(begin_lambda, 2) + -0.356237 *begin_x*lens_ipow(begin_dx, 2)*begin_lambda + 0.838616 *begin_x*lens_ipow(begin_lambda, 3) + 4.95014e-06 *lens_ipow(begin_y, 4)*begin_dx + 7.55289 *lens_ipow(begin_dx, 5) + 0.310712 *begin_x*lens_ipow(begin_dy, 4) + -0.0272238 *lens_ipow(begin_x, 2)*begin_dx*lens_ipow(begin_dy, 2) + -0.000405637 *lens_ipow(begin_x, 3)*lens_ipow(begin_dx, 2) + -0.0016729 *lens_ipow(begin_x, 2)*begin_y*begin_dx*begin_dy*begin_lambda + 1.38156e-06 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 2)*lens_ipow(begin_lambda, 2) + -5.59676e-09 *lens_ipow(begin_x, 5)*lens_ipow(begin_y, 2) + -2.08299e-09 *lens_ipow(begin_x, 7) + 1.51037e-09 *lens_ipow(begin_x, 7)*begin_lambda + 1.62764 *begin_x*lens_ipow(begin_dx, 4)*lens_ipow(begin_lambda, 3) + -2.43877e-11 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 6) + -0.000166531 *lens_ipow(begin_y, 4)*lens_ipow(begin_dx, 5) + 3.91242e-09 *begin_x*lens_ipow(begin_y, 7)*lens_ipow(begin_dx, 2)*begin_dy;
  pred_y =  + 26.6478 *begin_dy + 7.07798 *begin_dy*begin_lambda + 1.14323 *begin_y*begin_lambda + -0.180053 *begin_y*lens_ipow(begin_dx, 2) + -0.142826 *begin_y*lens_ipow(begin_dy, 2) + -0.0529828 *begin_x*begin_dx*begin_dy + -15.8269 *lens_ipow(begin_dx, 2)*begin_dy + -1.77677 *begin_y*lens_ipow(begin_lambda, 2) + -0.000519123 *lens_ipow(begin_x, 2)*begin_y + -4.90498 *begin_dy*lens_ipow(begin_lambda, 2) + -0.000503188 *lens_ipow(begin_y, 3) + 0.00136072 *lens_ipow(begin_x, 2)*begin_dy + -16.844 *lens_ipow(begin_dy, 3) + 0.931493 *begin_y*lens_ipow(begin_lambda, 3) + 0.000190732 *lens_ipow(begin_y, 3)*begin_lambda + 0.0001998 *lens_ipow(begin_x, 2)*begin_y*begin_lambda + -0.000822313 *begin_x*lens_ipow(begin_y, 2)*begin_dx*begin_dy + 0.318617 *begin_y*lens_ipow(begin_dx, 4) + 6.93717 *lens_ipow(begin_dy, 5) + -3.41864e-07 *lens_ipow(begin_x, 4)*begin_y + -0.00699567 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 3) + -0.000951 *lens_ipow(begin_y, 3)*lens_ipow(begin_dy, 2) + 0.000114581 *lens_ipow(begin_x, 3)*begin_y*begin_dx*lens_ipow(begin_dy, 2) + -1.3737e-09 *lens_ipow(begin_y, 7) + 88.5367 *lens_ipow(begin_dx, 4)*lens_ipow(begin_dy, 3) + -4.94822e-09 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 5) + -1.54899e-11 *lens_ipow(begin_x, 6)*lens_ipow(begin_y, 3) + -0.00168031 *lens_ipow(begin_y, 4)*lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 3);
  pred_dx =  + 0.399786 *begin_dx + -0.0374335 *begin_x + 0.0213387 *begin_x*begin_lambda + -0.0222137 *begin_y*begin_dx*begin_dy + 0.00011936 *begin_x*begin_y*begin_dy + -0.491997 *lens_ipow(begin_dx, 3) + -0.0165778 *begin_x*lens_ipow(begin_lambda, 2) + -0.483482 *begin_dx*lens_ipow(begin_dy, 2) + -2.52176e-05 *begin_x*lens_ipow(begin_y, 2) + -2.76551e-05 *lens_ipow(begin_x, 3) + -0.0329376 *begin_x*lens_ipow(begin_dx, 2) + -0.0367118 *begin_x*lens_ipow(begin_dy, 2)*begin_lambda + 2.08498e-06 *lens_ipow(begin_x, 4)*begin_dx + -4.32665e-08 *begin_x*lens_ipow(begin_y, 4) + 2.38937e-06 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 2)*begin_dx + 1.53062e-06 *lens_ipow(begin_x, 3)*begin_y*begin_dy + 1.87765e-06 *begin_x*lens_ipow(begin_y, 3)*begin_dy + 0.0326943 *begin_x*lens_ipow(begin_dy, 2)*lens_ipow(begin_lambda, 2) + 5.17241e-08 *begin_x*lens_ipow(begin_y, 4)*begin_lambda + 8.80235e-08 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 2)*begin_lambda + -5.13369e-10 *lens_ipow(begin_x, 5)*lens_ipow(begin_y, 2) + -9.63804e-08 *lens_ipow(begin_x, 5)*lens_ipow(begin_dy, 2) + 4.36787e-08 *lens_ipow(begin_x, 5)*lens_ipow(begin_lambda, 2) + 4.61028e-12 *lens_ipow(begin_y, 8)*begin_dx + -2.84439e-13 *lens_ipow(begin_x, 9) + -1.33303e-12 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 6) + -3.14982e-09 *lens_ipow(begin_x, 3)*lens_ipow(begin_y, 4)*lens_ipow(begin_dx, 2) + 2.94026e-08 *lens_ipow(begin_x, 6)*lens_ipow(begin_dx, 3);
  pred_dy =  + -0.0358994 *begin_y + 0.396945 *begin_dy + 0.0169134 *begin_y*begin_lambda + -0.0119194 *begin_y*lens_ipow(begin_dx, 2) + 0.000274491 *lens_ipow(begin_y, 2)*begin_dy + -0.030044 *begin_y*lens_ipow(begin_dy, 2) + -0.0217624 *begin_x*begin_dx*begin_dy + 0.000303076 *begin_x*begin_y*begin_dx + -0.491456 *lens_ipow(begin_dx, 2)*begin_dy + -0.0139602 *begin_y*lens_ipow(begin_lambda, 2) + -3.47907e-05 *lens_ipow(begin_x, 2)*begin_y + -3.48187e-05 *lens_ipow(begin_y, 3) + -0.4821 *lens_ipow(begin_dy, 3) + 1.8503e-05 *lens_ipow(begin_y, 3)*begin_lambda + 1.95796e-05 *lens_ipow(begin_x, 2)*begin_y*begin_lambda + 0.0103834 *begin_y*lens_ipow(begin_dx, 4) + -4.48971e-05 *lens_ipow(begin_x, 2)*begin_y*lens_ipow(begin_dy, 2) + 2.71146e-09 *lens_ipow(begin_y, 6)*begin_dy + -9.43117e-08 *lens_ipow(begin_y, 5)*lens_ipow(begin_dx, 2) + 4.17668e-06 *lens_ipow(begin_x, 4)*lens_ipow(begin_dx, 2)*begin_dy + -2.58285e-07 *lens_ipow(begin_y, 5)*lens_ipow(begin_dy, 2) + -1.19747e-10 *lens_ipow(begin_x, 6)*begin_y + 2.96507e-09 *begin_x*lens_ipow(begin_y, 5)*begin_dx + -3.63437e-10 *lens_ipow(begin_x, 2)*lens_ipow(begin_y, 5) + -2.40231e-13 *lens_ipow(begin_y, 9) + 1.48883e-10 *lens_ipow(begin_x, 4)*lens_ipow(begin_y, 4)*begin_dy + 1.09134e-10 *lens_ipow(begin_x, 5)*lens_ipow(begin_y, 3)*begin_dx + 3.74579e-12 *lens_ipow(begin_x, 8)*begin_dy;
  float dx1_domega0[2][2];
  dx1_domega0[0][0] =  + 26.7119  + 6.60947 *begin_lambda + -0.0547589 *begin_y*begin_dy + 0.00189755 *lens_ipow(begin_x, 2) + -49.6076 *lens_ipow(begin_dx, 2) + -14.2808 *lens_ipow(begin_dy, 2) + -4.54335 *lens_ipow(begin_lambda, 2) + -0.712474 *begin_x*begin_dx*begin_lambda + 4.95014e-06 *lens_ipow(begin_y, 4) + 37.7644 *lens_ipow(begin_dx, 4) + -0.0272238 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 2) + -0.000811273 *lens_ipow(begin_x, 3)*begin_dx + -0.0016729 *lens_ipow(begin_x, 2)*begin_y*begin_dy*begin_lambda + 6.51054 *begin_x*lens_ipow(begin_dx, 3)*lens_ipow(begin_lambda, 3) + -0.000832657 *lens_ipow(begin_y, 4)*lens_ipow(begin_dx, 4) + 7.82484e-09 *begin_x*lens_ipow(begin_y, 7)*begin_dx*begin_dy+0.0f;
  dx1_domega0[0][1] =  + -0.0547589 *begin_y*begin_dx + -28.5616 *begin_dx*begin_dy + -0.369321 *begin_x*begin_dy + 1.24285 *begin_x*lens_ipow(begin_dy, 3) + -0.0544476 *lens_ipow(begin_x, 2)*begin_dx*begin_dy + -0.0016729 *lens_ipow(begin_x, 2)*begin_y*begin_dx*begin_lambda + 3.91242e-09 *begin_x*lens_ipow(begin_y, 7)*lens_ipow(begin_dx, 2)+0.0f;
  dx1_domega0[1][0] =  + -0.360107 *begin_y*begin_dx + -0.0529828 *begin_x*begin_dy + -31.6538 *begin_dx*begin_dy + -0.000822313 *begin_x*lens_ipow(begin_y, 2)*begin_dy + 1.27447 *begin_y*lens_ipow(begin_dx, 3) + 0.000114581 *lens_ipow(begin_x, 3)*begin_y*lens_ipow(begin_dy, 2) + 354.147 *lens_ipow(begin_dx, 3)*lens_ipow(begin_dy, 3) + -0.00336062 *lens_ipow(begin_y, 4)*begin_dx*lens_ipow(begin_dy, 3)+0.0f;
  dx1_domega0[1][1] =  + 26.6478  + 7.07798 *begin_lambda + -0.285652 *begin_y*begin_dy + -0.0529828 *begin_x*begin_dx + -15.8269 *lens_ipow(begin_dx, 2) + -4.90498 *lens_ipow(begin_lambda, 2) + 0.00136072 *lens_ipow(begin_x, 2) + -50.532 *lens_ipow(begin_dy, 2) + -0.000822313 *begin_x*lens_ipow(begin_y, 2)*begin_dx + 34.6859 *lens_ipow(begin_dy, 4) + -0.020987 *lens_ipow(begin_x, 2)*lens_ipow(begin_dy, 2) + -0.001902 *lens_ipow(begin_y, 3)*begin_dy + 0.000229162 *lens_ipow(begin_x, 3)*begin_y*begin_dx*begin_dy + 265.61 *lens_ipow(begin_dx, 4)*lens_ipow(begin_dy, 2) + -0.00504093 *lens_ipow(begin_y, 4)*lens_ipow(begin_dx, 2)*lens_ipow(begin_dy, 2)+0.0f;
  float invJ[2][2];
  const float invdet = 1.0f/(dx1_domega0[0][0]*dx1_domega0[1][1] - dx1_domega0[0][1]*dx1_domega0[1][0]);
  invJ[0][0] =  dx1_domega0[1][1]*invdet;
  invJ[1][1] =  dx1_domega0[0][0]*invdet;
  invJ[0][1] = -dx1_domega0[0][1]*invdet;
  invJ[1][0] = -dx1_domega0[1][0]*invdet;
  const float dx1[2] = {out_x - pred_x, out_y - pred_y};
  for(int i=0;i<2;i++)
  {
    dx += invJ[0][i]*dx1[i];
    dy += invJ[1][i]*dx1[i];
  }
  sqr_err = dx1[0]*dx1[0] + dx1[1]*dx1[1];
}
out_dx = pred_dx;
out_dy = pred_dy;
