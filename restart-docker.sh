#!/bin/bash
imageName=enic
containerName=enicdk

docker build -t $imageName -f Dockerfile  .

echo Stop old container...
docker stop $containerName

echo Delete old container...
docker rm -f $containerName

echo Run new container...
docker run -d -p 3030:3030 --name $containerName $imageName
