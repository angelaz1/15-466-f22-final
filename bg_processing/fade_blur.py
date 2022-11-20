import cv2
import numpy as np
import sys
import matplotlib.pyplot as plt


im = cv2.imread("bg_processing/test.jpg")
# blur using progressively larger kernels
# for i in range(1, 20, 2):
#     im = cv2.GaussianBlur(im, (i, i), 0)
#     cv2.imshow("blur", im)
#     cv2.waitKey(0)
#     cv2.destroyAllWindows()

# blur by blending with a blurred version of the image
imblur = cv2.GaussianBlur(im, (21, 21), 0)
for i in range(1, 20, 2):
    im = cv2.addWeighted(im, 1.0 - i / 20.0, imblur, i / 20.0, 0)
    cv2.imshow("blur", im)
    cv2.waitKey(0)
    cv2.destroyAllWindows()