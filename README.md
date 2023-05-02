**Scalable Computing Assignment**

**Edge detection by applying Stencil code on Image**

In this assignment we will use a Stencil code. An Image is considered to be a matrix of
pixels. And stencil operators take an input image and convolve it with Stencil kernel to
produce the output image according to the following formula:

We will use at nine point stencil that performs **edge detection**. **Qx,y** is a pixel of the
output image, which lies at the intersection of the row x and the column y. **Px,y** is a pixel
of the input image, which lies at the intersection of the row x and the column y. Indices
of P and Q shows the position of the pixels in the input image and the output image,
respectively.

**Cij** also shows the elements of the 3\*3 stencil matrix showed in the following ﬁgure:

In the basic implementation, we could keep our input and output images as arrays of
ﬂoating point numbers. To apply the Stencil, we may set up two arrays that traverse all
pixels in the image and for each pixel we take the pixel and its eight neighbors, multiply
them by the weights in the Stencil matrix, add them, and this produces the output
image value.


<a name="br2"></a>We have to then check whether this value is within the bounds of our intensity depth.
Speciﬁcally this brightness has to be between 0 and 255 for an 8bit grey scale image.

The task is to implement an initial serial version of the program, where it takes an
image as an input and then produces an output image after applying the stencil matrix
on the input image. The code is optimized the in two levels:
 · Firstly, using openMP a multithreaded version of the program is created.

· Secondly, the code is  further optimizes using MPI in order to
 distribute the computations to diﬀerent compute nodes in the cluster in the
 Linux Pool.
