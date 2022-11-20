import cv2
import numpy as np
import sys
import matplotlib.pyplot as plt

def pixelate_image (img):
    imgshape = img.shape
    rows, cols, _ = imgshape

    dim = (rows / 4, cols / 4)
    img = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)
    img = cv2.resize(img, (rows, cols), interpolation = cv2.INTER_NEAREST)

    cv2.imshow("pixelated", img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()

    ret = np.zeros(imgshape)

    

    for i in range(rows):
        for j in range(cols):
            # get pixel
            row = i // 4 * 4
            col = j // 4 * 4
            color = img[row, col]
            ret[i, j] = color // 32 * 32
    return ret

im = cv2.imread("test.jpg")
resized = cv2.resize(im, (im.shape[1] // 4, im.shape[0] // 4), interpolation = cv2.INTER_AREA)
resized = cv2.resize(resized, (im.shape[1], im.shape[0]), interpolation = cv2.INTER_NEAREST)

cv2.imshow("pixelated", resized)
cv2.waitKey(0)
cv2.destroyAllWindows()

resized = cv2.GaussianBlur(resized, (5, 5), 0)

kernel = np.array([[0, -1, 0],
                   [-1, 5,-1],
                   [0, -1, 0]])
resized = cv2.filter2D(resized, -1, kernel)
resized = cv2.filter2D(resized, -1, kernel)


cv2.imshow("sharpened", resized)
cv2.waitKey(0)
cv2.destroyAllWindows()



