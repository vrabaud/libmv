/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2009, Willow Garage, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Willow Garage, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <opencv2/sfm/sfm.hpp>

/* libmv headers */
#include <libmv/reconstruction/reconstruction.h>
#include <libmv/reconstruction/projective_reconstruction.h>

using namespace cv;
using namespace libmv;
using namespace std;

void
reconstruct(const InputArrayOfArrays points2d, OutputArrayOfArrays projection_matrices, OutputArray points3d, bool,
            bool is_projective, bool has_outliers, bool is_sequence)
{
  bool result = false;

  /* Data types needed by libmv functions */
  Matches matches;
  Matches *matches_inliers;
  Reconstruction recon;

  /* Convert OpenCV types to libmv types */
  unsigned int nviews = (unsigned) points2d.total();
  for (int v = 0; v < nviews; ++v)
  {
    std::vector<cv::Point2d> imgpts = points2d.getMat(v);
    for (int p = 0; p < imgpts.size(); ++p)
    {
      cv::Point2d pt = imgpts[p];
      PointFeature * feature = new PointFeature(pt.x, pt.y);
      matches.Insert(v, p, feature);
    }
  }

  /* Projective reconstruction*/
  if (is_projective)
  {
    /* Two view reconstruction */
    if (nviews == 2)
      result = ReconstructFromTwoUncalibratedViews(matches, 0, 1, matches_inliers, &recon);

    /* Set output data */
    /* Get Cameras */
    CV_Assert(recon.GetNumberCameras()==nviews);
    libmv::PinholeCamera *cam;
    libmv::Mat34 P;
    projection_matrices.create(3, 4, CV_32FC1, nviews);

    for (int i = 0; i < nviews; ++i)
    {
      cam = (PinholeCamera *) recon.GetCamera(i);
      P = cam->GetPoseMatrix();
      cv::Mat Pcv = projection_matrices.getMat(i);
      eigen2cv(P, Pcv);
//      cout << Pcv << endl; // not impl? - print

    }

    /*    3D structure*/
    // where is the 3D pt info? in tracks?? check Recon...code
    /*
     PointStructure *ptstruc;
     Vec3 pt3;
     for (int i = 0; i < recon.GetNumberStructures(); ++i)
     {
     ptstruc = (PointStructure)recon.GetStructure(i);

     cout << "3D pt" << ptstruc->coords_affine() << endl;
     }
     */

  }
  /* Euclidian reconstruction*/
  else
  {

  }

  /* Give error if reconstruction failed */
  CV_Assert(result==true);
}
