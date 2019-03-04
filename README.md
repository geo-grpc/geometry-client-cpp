From inside repo directory

```bash
protoc --cpp_out ./geometry/src -I ./proto  ./proto/geometry_operators.proto ./proto/geometry.proto
```

```bash
protoc --grpc_out=./geometry/src -I ./proto  --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin ./proto/geometry_operators.proto
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
