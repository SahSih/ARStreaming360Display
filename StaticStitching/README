# Static Stitching

## Introduction

Static stitching is the process of taking an arbitrary number of still images and stitching them 
together into a panorama view. Another feature currently being implemented with static stitching
is the ability to take an arbitrary number of saved video feeds and stitch those video feeds into
a panorama video view.

In this tutorial, you will learn to use a program called "imageStitching.cpp", which is based on 
OpenCV's image stitching code example to stitch together an arbitrary number of photos.

## Prerequisites
- Installed Ubuntu 16.04 Baremetal or Virtual Machine
- Downloaded and Installed Latest Version of OpenCV
    - If you don't have OpenCV already set up, download our [install_opencv.sh](install_opencv.sh) script
    - Run the script `sh -x install_opencv.sh`
- Downloaded our Makefile to build OpenCV c++ source code into executable programs
    - [Makefile](Makefile)
  
## Run "imageStitching.cpp" via terminal

> Note: You should have **OpenCV** installed on your computer and have downloaded the **Makefile**. 
Makefile should be placed into the same folder where your OpenCV source code files are located.

1\. Download [imageStitching.cpp](imageStitching.cpp)

2\. Wherever your **collection of burst camera photos** are, place that folder that holds them into the same
location as the imageStitching.cpp program.

3\. Let's compile the program using Makefile. Type/paste the command below into your terminal. Also make sure
you are in the same folder as "imageStitching.cpp".

~~~
make imageStitching
~~~

4\. Feel free to read the source code and learn to interact with the program. I will also provide you with a 
walkthrough of how to interact with the program via terminal.

First we will look at the command line user interface menu that this program provides:

- As you can see this is a large list of features. Let's break it down: 
    - to the right of **stitching_detailed**, 
        - it shows you can insert **images** into the program just by writing their **specific file name**. 
    - You also have **flags, Motion Estimation Flags and Compositing flags**. 
        - These options enable you to configure how the photos are processed to create multiple different panorama views.

5\. Briefly analyze the menu options below, then we will walkthrough how to execute the program from the command line.
 
~~~cpp
static void printUsage()
{
    cout <<
        "Rotation model images stitcher.\n\n"
        "stitching_detailed img1 img2 [...imgN] [flags]\n\n"
        "Flags:\n"
        "  --preview\n"
        "      Run stitching in the preview mode. Works faster than usual mode,\n"
        "      but output image will have lower resolution.\n"
        "  --try_cuda (yes|no)\n"
        "      Try to use CUDA. The default value is 'no'. All default values\n"
        "      are for CPU mode.\n"
        "\nMotion Estimation Flags:\n"
        "  --work_megapix <float>\n"
        "      Resolution for image registration step. The default is 0.6 Mpx.\n"
        "  --features (surf|orb)\n"
        "      Type of features used for images matching. The default is surf.\n"
        "  --matcher (homography|affine)\n"
        "      Matcher used for pairwise image matching.\n"
        "  --estimator (homography|affine)\n"
        "      Type of estimator used for transformation estimation.\n"
        "  --match_conf <float>\n"
        "      Confidence for feature matching step. The default is 0.65 for surf and 0.3 for orb.\n"
        "  --conf_thresh <float>\n"
        "      Threshold for two images are from the same panorama confidence.\n"
        "      The default is 1.0.\n"
        "  --ba (no|reproj|ray|affine)\n"
        "      Bundle adjustment cost function. The default is ray.\n"
        "  --ba_refine_mask (mask)\n"
        "      Set refinement mask for bundle adjustment. It looks like 'x_xxx',\n"
        "      where 'x' means refine respective parameter and '_' means don't\n"
        "      refine one, and has the following format:\n"
        "      <fx><skew><ppx><aspect><ppy>. The default mask is 'xxxxx'. If bundle\n"
        "      adjustment doesn't support estimation of selected parameter then\n"
        "      the respective flag is ignored.\n"
        "  --wave_correct (no|horiz|vert)\n"
        "      Perform wave effect correction. The default is 'horiz'.\n"
        "  --save_graph <file_name>\n"
        "      Save matches graph represented in DOT language to <file_name> file.\n"
        "      Labels description: Nm is number of matches, Ni is number of inliers,\n"
        "      C is confidence.\n"
        "\nCompositing Flags:\n"
        "  --warp (affine|plane|cylindrical|spherical|fisheye|stereographic|compressedPlaneA2B1|compressedPlaneA1.5B1|compressedPlanePortraitA2B1|compressedPlanePortraitA1.5B1|paniniA2B1|paniniA1.5B1|paniniPortraitA2B1|paniniPortraitA1.5B1|mercator|transverseMercator)\n"
        "      Warp surface type. The default is 'spherical'.\n"
        "  --seam_megapix <float>\n"
        "      Resolution for seam estimation step. The default is 0.1 Mpx.\n"
        "  --seam (no|voronoi|gc_color|gc_colorgrad)\n"
        "      Seam estimation method. The default is 'gc_color'.\n"
        "  --compose_megapix <float>\n"
        "      Resolution for compositing step. Use -1 for original resolution.\n"
        "      The default is -1.\n"
        "  --expos_comp (no|gain|gain_blocks)\n"
        "      Exposure compensation method. The default is 'gain_blocks'.\n"
        "  --blend (no|feather|multiband)\n"
        "      Blending method. The default is 'multiband'.\n"
        "  --blend_strength <float>\n"
        "      Blending strength from [0,100] range. The default is 5.\n"
        "  --output <result_img>\n"
        "      The default is 'result.jpg'.\n"
        "  --timelapse (as_is|crop) \n"
        "      Output warped images separately as frames of a time lapse movie, with 'fixed_' prepended to input file names.\n"
        "  --rangewidth <int>\n"
        "      uses range_width to limit number of images to match with.\n";
}
~~~

6\. Example of how to enter image file names into the program:

~~~
./imageStitching sjsuTowerLawn1.jpg sjsuTowerLawn2.jpg sjsuTowerLawn3.jpg sjsuTowerLawn4.jpg
~~~

> Note: You must specify **at least 4 images** for the program to run successfully.

~~~
./imageStitching sjsuTowerLawn*
~~~

What does the above command do?
- It takes all the files in your folder that start with **sjsuTowerLawn** and inserts them into the program. 

The default panorama visualization is a spherical view. Check the same folder the program is located in and you should see
a file called **result**.


> Note: The program has features set by default, the minimum you need to enter is the file names. 

7\. You can also enter the feature detection algorithms you want to use along with the warping algorithms in addition to other features.

~~~
./imageStitching sjsuTowerLawn* --features orb --warp stereographic --output sjsu360pano.jpg
~~~

The above command sets the images we want to use, the feature algorithm, warping algorithm stereographic view for panorama,
and the output filename is called "sjsu360pano.jpg".




    

