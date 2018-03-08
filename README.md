```bash
protoc -I ./protos --grpc_out=./geometry --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./protos/geometry_operators.proto
```
```bash
protoc -I ./protos --cpp_out=./geometry ./protos/geometry_operators.proto 
```



## Testing

Start Service:
```bash
docker run -p 8980:8980 -it --network=geometry-network --name=temp-c us.gcr.io/echoparklabs/geometry-service-java
```

Get ip address:
```bash
sudo docker inspect --format='{{range .NetworkSettings.Networks}}{{.IPAddress}}{{end}}' temp-c
```

Use ip address to run tests:
```bash
sudo docker run -it --network=geometry-network -e GEOMETRY_SERVICE_HOST=172.18.0.2:8980 temp-i /bin/bash -c "cd geometry-test; ./unitTest"
```