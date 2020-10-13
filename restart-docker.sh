#!/bin/bash
imageName=enicimage
containerName=dockerenic

docker build -t $imageName -f Dockerfile  .

echo Stop old container...
docker stop $containerName

echo Delete old container...
docker rm -f $containerName

echo Run new container...
docker run -d -p 3000:3000 --name $containerName $imageName
