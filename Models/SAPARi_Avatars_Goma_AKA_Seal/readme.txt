Goma avatar from SAPARi

The original model doesn't use a texture (despite having proper UVs), but rather each color has its own object and material. To clean things up, and to make sure it can be opened in as many programs as possible, they've all been unified under one material using a single texture.

Make sure the texture is unfiltered ("Closest" in Blender)

The DAE file includes the original transformation origins (since it doesn't use bones)

