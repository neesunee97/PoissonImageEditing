# PoissonImageEditing, SIGGRAPH(2003)
Patrick P'erez, Michel Gangnet, and Andrew Blake(Microsoft Research UK)
https://www.cs.jhu.edu/~misha/Fall07/Papers/Perez03.pdf

## Seamless cloning
Seamless cloning is used when cropping a desired area from the source image and composing it on the destination image.
It automatically smooths the boundary and adjusts the overall tone.
<br>👇👇👇Like this!👇👇👇<br>
![image](https://user-images.githubusercontent.com/70381416/130940868-fe62eafb-724e-44c2-b05f-a77037b09727.png)

---

![SE-7134dc0e-bd02-4ccd-ac2e-45b9236e4571](https://user-images.githubusercontent.com/70381416/130943070-210a4e92-d58a-4dc6-9b8f-e469cf24afac.png)<br>
↑This is the picture that explain the overall Poisson Image Editing.<br>
Ω is the area within the mask for both the corresponding pixel and 4 neighbor pixels.
When the corresponding pixel is inside the mask, it is called the boundary, ∂Ω , if at least one of the 4 neighbor pixels is outside the mask.<br>
![image (1)](https://user-images.githubusercontent.com/70381416/130945676-12a1deea-d787-410d-8c05-13135648f607.png)
<br><br>
This method is ultimately a matter of applying the following equation to each pixel.<br>![화면_캡처_2021-08-10_204704](https://user-images.githubusercontent.com/70381416/130946330-b3a98675-9fdd-4a26-aef1-a441612f8933.png)<br>
We need not a equation that is continuous, but a equation that is discrete. So this paper applied the following equation.<br>
<img src = "https://user-images.githubusercontent.com/70381416/130946858-7a6774e2-fba4-4f4b-a016-64566fbbdce9.png"><br>

Np, fq, f*q and Vpq are kown, but fp is unknow. So, by transposing known values to the right side and translating unknown values to the left side, the following equation can be created.<br>
<img src = "https://user-images.githubusercontent.com/70381416/130947768-a830506f-b205-4075-893b-e035dd423acd.jpg" width = "50%" height = "50%"><br>

In this paper, the Gauss-seidel iteration was used to obtain the fp value. Therefore, we will iterate several times to get the value.

There are two methods to apply a gradient in seamless cloning. One is the importing gradient and the other is the mixing gradient.
The results are slightly different depending on which gradient is given.

### Importing gradiennt
- To conceal indesirable image features
- To insert new elements in an image
<br><br>Its gradient equation is the following.<br>
<img src = "https://user-images.githubusercontent.com/70381416/130950700-656b2f30-f0e9-4101-b5ed-4563c44d8f1b.png"><br>
This gradient used only the value of the source pixels. So A problem arises like the following picture.
<br><img src = "https://user-images.githubusercontent.com/70381416/130951181-31f9074a-11cd-446b-9605-b23ad8e75c6f.png"><br>

As shown in the picture on the upper right, the background is blurred. It can solve by using Mixing gradient.
  

### Mixing gradient
This method adopts the more intense value between the gradient of the destination and the gradient of the source. It can be used when objects overlap or when compositing translucent objects. The formula is used below.<br>
<img src = "https://user-images.githubusercontent.com/70381416/130952071-be81b2c7-31e8-4ac0-ba84-50160800a86f.png"><br>
You can use the appropriate one of the two methods.

<br>

### Result
<img src = "https://user-images.githubusercontent.com/70381416/130952429-2e3add6e-327f-477c-839d-5a0859c86ef7.jpg" width = "45%" height = "45%"> <img src = "https://user-images.githubusercontent.com/70381416/130952432-3ee1f390-bdaa-4094-a20d-975e5e3c747f.jpg" /><br>

The desired area in the left photo were cropped out and the method was applied to the right photo.<br><br>
![Final_result (2)](https://user-images.githubusercontent.com/70381416/130953753-1f9e076b-a959-4ddf-952b-e3d2472321ad.png)<br><br><br>

## Selecting editing
Selection editing can change the illumination, color or texture of the desired area in the source image. Also, it can apply seamless tiling by pasting one picture together.
**But this post does not implement this method.**

<br><br><br><br>
# Recently research
This paper was published in 2003. It has evolved a lot over the past 20 years with deep learning.
<br>
![image (3)](https://user-images.githubusercontent.com/70381416/130954113-42c3e027-4028-4a91-ac42-814e246d39cf.png)<br><br>
By applying deep learning, a method was even made to apply the textures and the feel of the images above.
