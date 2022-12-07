# Copyright (C) Deliever (https://deliever42.me)

import pip
import argparse

try:
    import cv2
except ImportError:
    pip.main(['install', 'opencv-python'])
    import cv2

try:
    import numpy
except ImportError:
    pip.main(['install', 'numpy'])
    import numpy

prototxtPath = 'models/colorization_deploy_v2.prototxt'
modelPath = 'models/colorization_release_v2.caffemodel'
kernelPath = 'kernel/pts_in_hull.npy'

parser = argparse.ArgumentParser()

parser.add_argument(
    "--imagePath", help="Path to the image to be colorized", type=str, required=True)
parser.add_argument("--destinationPath",
                    help="Path to the destination of the colorized image", type=str, required=True)

args = parser.parse_args()

imagePath = args.imagePath
destination = args.destinationPath

net = cv2.dnn.readNetFromCaffe(prototxtPath, modelPath)
pts = numpy.load(kernelPath).transpose().reshape(2, 313, 1, 1)

net.getLayer(net.getLayerId('class8_ab')).blobs = [
    pts.astype('float32')]
net.getLayer(net.getLayerId('conv8_313_rh')).blobs = [
    numpy.full([1, 313], 2.606, dtype='float32')]

bwImage = cv2.imread(imagePath)
scaledImage = bwImage.astype('float32') / 255.0
labImage = cv2.cvtColor(scaledImage, cv2.COLOR_BGR2LAB)

resizedImage = cv2.resize(labImage, (224, 224))
L = cv2.split(resizedImage)[0] - 50

net.setInput(cv2.dnn.blobFromImage(L))
abImage = cv2.resize(net.forward()[0, :, :, :].transpose(
    (1, 2, 0)), (bwImage.shape[1], bwImage.shape[0]))
L = cv2.split(labImage)[0]

colorizedImage = numpy.concatenate(
    (L[:, :, numpy.newaxis], abImage), axis=2)
colorizedImage = cv2.cvtColor(colorizedImage, cv2.COLOR_LAB2BGR)
colorizedImage = (255 * colorizedImage).astype('uint8')

cv2.imwrite(destination, colorizedImage)
cv2.destroyAllWindows()

exit()