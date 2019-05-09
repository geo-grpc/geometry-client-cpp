//
// Created by David Raleigh on 3/7/18.
//

/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <string>

#include <grpcpp/grpcpp.h>
#include <grpc/support/log.h>
#include <thread>

//#include "../geometry/geometry_operators.grpc.pb.h"
//#include "../geometry/geometry_operators.pb.h"
#include "epl/protobuf/geometry.pb.h"
#include "epl/protobuf/geometry_service.grpc.pb.h"

#include "gtest/gtest.h"
#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <cstdlib>
#include <memory>
#include <regex>

using namespace epl::protobuf;

using grpc::Channel;
using grpc::ClientReader;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::Status;

namespace {
    class GeometryClientTest : public ::testing::Test {
    protected:
        GeometryClientTest() {

        }
    };

    TEST_F(GeometryClientTest, TEST_1) {
        EXPECT_NE(1, -1);
    }

    TEST_F(GeometryClientTest, TEST_CUT_1){
        std::shared_ptr<grpc::Channel> channel;
        if (const char* env_p = std::getenv("GEOMETRY_SERVICE_HOST")) {
            channel = grpc::CreateChannel(env_p, grpc::InsecureChannelCredentials());
        } else {
            channel = grpc::CreateChannel("localhost:8980", grpc::InsecureChannelCredentials());
        }

        std::unique_ptr<GeometryService::Stub> geometry_stub = GeometryService::NewStub(channel);

        GeometryData serviceGeometry;
        const char* wkt = "MULTIPOLYGON (((40 40, 20 45, 45 30, 40 40)), ((20 35, 45 20, 30 5, 10 10, 10 30, 20 35), (30 20, 20 25, 20 15, 30 20)))";
        serviceGeometry.set_wkt(wkt);

        GeometryData cutterGeometry;
        const char* wkt_cutter = "LINESTRING(0 0, 45 45)";
        cutterGeometry.set_wkt(wkt_cutter);

        GeometryRequest operatorRequest;
        operatorRequest.mutable_left_geometry()->CopyFrom(serviceGeometry);
        operatorRequest.mutable_right_geometry()->CopyFrom(cutterGeometry);
        operatorRequest.set_operator_(OperatorType::CUT);
        operatorRequest.set_result_encoding(Encoding::WKT);
        GeometryResponse geometryResponse;
        ClientContext context;

        std::cout << "Looking for features between 40, -75 and 42, -73"
                  << std::endl;

        std::unique_ptr<ClientReader<GeometryResponse> > reader(
                geometry_stub->GeometryOperationServerStream(&context, operatorRequest));
        int count = 0;
        const char* expected1 = "MULTIPOLYGON (((35.625 35.625, 40 40, 20 45, 35.625 35.625)), ((10 10, 20 20, 20 25, 23.333333333333336 23.333333333333336, 29.375 29.375, 20 35, 10 30, 10 10)))";
        const char* expected2 = "MULTIPOLYGON (((40 40, 35.625 35.625, 45 30, 40 40)), ((30 5, 45 20, 29.375 29.375, 23.333333333333336 23.333333333333336, 30 20, 20 15, 20 20, 10 10, 30 5)))";
        while (reader->Read(&geometryResponse)) {
            std::string result = geometryResponse.geometry().wkt();
            if (count == 0) {
                EXPECT_STREQ(expected1 , result.c_str());
            } else {
                EXPECT_STREQ(expected2 , result.c_str());
            }

            count ++;
        }
        EXPECT_EQ(2, count);
        Status status = reader->Finish();
        if (status.ok()) {
            std::cout << "ListFeatures rpc succeeded." << std::endl;
        } else {
            EXPECT_NO_THROW(status.error_message());
        }
    }

    TEST_F(GeometryClientTest, TEST_2) {
        std::shared_ptr<grpc::Channel> channel;
        if (const char* env_p = std::getenv("GEOMETRY_SERVICE_HOST")) {
            channel = grpc::CreateChannel(env_p, grpc::InsecureChannelCredentials());
        } else {
            channel = grpc::CreateChannel("localhost:8980", grpc::InsecureChannelCredentials());
        }
        std::unique_ptr<GeometryService::Stub> geometry_stub = GeometryService::NewStub(channel);

        SpatialReferenceData spatialReferenceWGS84;
        spatialReferenceWGS84.set_wkid(4326);

        auto* spatialReferenceCalif = new SpatialReferenceData();
        spatialReferenceCalif->set_wkid(32632);

        // allocating this here means it is not copied in the set_allocated method, but a strange rule of control is given to the
        // operator request message
        auto* serviceGeometry = new GeometryData();
        const char* wkt = "MULTILINESTRING ((500000       0, 400000  100000, 600000 -100000))";
        serviceGeometry->set_wkt(wkt);
        serviceGeometry->set_allocated_sr(spatialReferenceCalif);

        auto* operatorRequest = new GeometryRequest();
        operatorRequest->mutable_result_sr()->CopyFrom(spatialReferenceWGS84);
        operatorRequest->set_allocated_left_geometry(serviceGeometry);
        operatorRequest->set_allocated_operation_sr(spatialReferenceCalif);
        operatorRequest->set_operator_(OperatorType::PROJECT);
        operatorRequest->set_result_encoding(Encoding::WKT);

        auto* clientContext = new grpc::ClientContext();
        auto* operatorResult = new GeometryResponse();

        geometry_stub->GeometryOperationUnary(clientContext, *operatorRequest, operatorResult);

        std::string result = operatorResult->geometry().wkt();
        std::string expected("MULTILINESTRING ((9 0, 8.101251062924646 0.904618578893133, 9.898748937075354 -0.904618578893133))");
        std::string expected_2("MULTILINESTRING ((9 0, 8.101251062924646 0.9046185788931331, 9.898748937075354 -0.9046185788931331))");
        if (strncmp(result.c_str(), expected.c_str(), expected.size()) != 0 &&
                strncmp(result.c_str(), expected_2.c_str(), expected_2.size()) != 0) {
            if (strncmp(result.c_str(), expected.c_str(), expected.size()) != 0)
                EXPECT_STREQ(expected.c_str() , result.c_str());
            else
                EXPECT_STREQ(expected_2.c_str() , result.c_str());
        }
    }

    TEST_F(GeometryClientTest, TEST_CRAZY_NESTING) {
        std::shared_ptr<grpc::Channel> channel;
        if (const char* env_p = std::getenv("GEOMETRY_SERVICE_HOST")) {
            channel = grpc::CreateChannel(env_p, grpc::InsecureChannelCredentials());
        } else {
            channel = grpc::CreateChannel("localhost:8980", grpc::InsecureChannelCredentials());
        }
        std::unique_ptr<GeometryService::Stub> geometry_stub = GeometryService::NewStub(channel);

        /*
         * Polyline polyline = new Polyline();
        polyline.startPath(-120, -45);
        polyline.lineTo(-100, -55);
        polyline.lineTo(-90, -63);
        polyline.lineTo(0, 0);
        polyline.lineTo(1, 1);
        polyline.lineTo(100, 25);
        polyline.lineTo(170, 45);
        polyline.lineTo(175, 65);
        */
        const char* polyline = "MULTILINESTRING ((-120 -45, -100 -55, -90 -63, 0 0, 1 1, 100 25, 170 45, 175 65))";

         /*OperatorExportToWkb op = OperatorExportToWkb.local();

        SpatialReferenceData spatialReferenceNAD = SpatialReferenceData.newBuilder().setWkid(4269).build();
        SpatialReferenceData spatialReferenceMerc = SpatialReferenceData.newBuilder().setWkid(3857).build();
        SpatialReferenceData spatialReferenceWGS = SpatialReferenceData.newBuilder().setWkid(4326).build();
        SpatialReferenceData spatialReferenceGall = SpatialReferenceData.newBuilder().setWkid(54016).build();
        //TODO why does esri shape fail
          */
        SpatialReferenceData spatialReferenceNAD;
        spatialReferenceNAD.set_wkid(4269);
        SpatialReferenceData spatialReferenceMerc;
        spatialReferenceMerc.set_wkid(3857);
        SpatialReferenceData spatialReferenceWGS;
        spatialReferenceWGS.set_wkid(4326);
        SpatialReferenceData spatialReferenceGall;
        spatialReferenceGall.set_wkid(54016);
/*
        GeometryBagData geometryBagLeft = GeometryBagData.newBuilder()
                .setGeometryEncodingType(GeometryEncodingType.wkb)
                .addGeometryBinaries(ByteString.copyFrom(op.execute(0, polyline, null)))
                .setSpatialReference(spatialReferenceNAD)
                .build();
                */
        auto* geometryBagLeft = new GeometryData();
        geometryBagLeft->set_allocated_sr(&spatialReferenceNAD);
        geometryBagLeft->set_wkt(polyline);

        auto* serviceOpLeft = new GeometryRequest();
        serviceOpLeft->set_allocated_left_geometry(geometryBagLeft);
        serviceOpLeft->set_operator_(OperatorType::BUFFER);
        GeometryRequest::BufferParams bufferParams;
        bufferParams.set_distance(.5);
        serviceOpLeft->set_allocated_buffer_params(&bufferParams);
        serviceOpLeft->mutable_result_sr()->CopyFrom(spatialReferenceWGS);

               /*
        GeometryRequest nestedLeft = GeometryRequest
                .newBuilder()
                .setLeftNestedRequest(serviceOpLeft)
                .setOperatorType(ServiceOperatorType.ConvexHull)
                .setResultSpatialReference(spatialReferenceGall)
                .build();
                */
        auto* nestedLeft = new GeometryRequest();
        nestedLeft->set_allocated_left_geometry_request(serviceOpLeft);
        nestedLeft->set_operator_(OperatorType::CONVEX_HULL);
        nestedLeft->mutable_result_sr()->CopyFrom(spatialReferenceGall);

                /*

        GeometryBagData geometryBagRight = GeometryBagData.newBuilder()
                .setGeometryEncodingType(GeometryEncodingType.wkb)
                .setSpatialReference(spatialReferenceNAD)
                .addGeometryBinaries(ByteString.copyFrom(op.execute(0, polyline, null)))
                .build();
                 */
        auto* geometryBagRight = new GeometryData();
        geometryBagRight->set_allocated_sr(&spatialReferenceNAD);
        geometryBagRight->set_wkt(polyline);
//        geometryBagRight->set_geometry_encoding_type(GeometryEncodingType::wkt);
//        geometryBagRight->add_geometry_strings(polyline);

                 /*
        GeometryRequest serviceOpRight = GeometryRequest
                .newBuilder()
                .setLeftGeometryBag(geometryBagRight)
                .setOperatorType(GeometryRequest.GeodesicBuffer)
                .setBufferParams(GeometryRequest.BufferParams.newBuilder().addDistances(1000).setUnionResult(false).build())
                .setOperationSpatialReference(spatialReferenceWGS)
                .build();
                  */
        auto* serviceOpRight = new GeometryRequest();
        serviceOpRight->set_allocated_left_geometry(geometryBagRight);
        serviceOpRight->set_operator_(OperatorType::GEODESIC_BUFFER);
        GeometryRequest::BufferParams geodesicBufferParams;
//        GeometryRequest_BufferParams geodesicBufferParams;
        geodesicBufferParams.set_distance(1000);
        geodesicBufferParams.set_union_result(false);
        serviceOpRight->set_allocated_buffer_params(&geodesicBufferParams);
        serviceOpRight->mutable_operation_sr()->CopyFrom(spatialReferenceWGS);

                  /*
        GeometryRequest nestedRight = GeometryRequest
                .newBuilder()
                .setLeftNestedRequest(serviceOpRight)
                .setOperatorType(GeometryRequest.ConvexHull)
                .setResultSpatialReference(spatialReferenceGall)
                .build();
                   */
        auto* nestedRight = new GeometryRequest();
        nestedRight->set_allocated_left_geometry_request(serviceOpRight);
        nestedRight->set_operator_(OperatorType::CONVEX_HULL);
        nestedRight->mutable_result_sr()->CopyFrom(spatialReferenceGall);


                   /*

        GeometryRequest operatorRequestContains = GeometryRequest
                .newBuilder()
                .setLeftNestedRequest(nestedLeft)
                .setRightNestedRequest(nestedRight)
                .setOperatorType(GeometryRequest.Contains)
                .setOperationSpatialReference(spatialReferenceMerc)
                .build();
                    */
        auto* operatorRequestContains = new GeometryRequest();
        operatorRequestContains->set_allocated_left_geometry_request(serviceOpLeft);
        operatorRequestContains->set_allocated_right_geometry_request(serviceOpRight);
        operatorRequestContains->set_operator_(OperatorType::CONTAINS);
        operatorRequestContains->mutable_operation_sr()->CopyFrom(spatialReferenceMerc);
                    /*

        GeometryServiceGrpc.GeometryServiceBlockingStub stub = GeometryServiceGrpc.newBlockingStub(inProcessChannel);
        GeometryResponse operatorResult = stub.executeOperation(operatorRequestContains);
        Map<Integer, Boolean> map = operatorResult.getRelateMapMap();
  */
        auto* clientContext = new grpc::ClientContext();
        auto* operatorResult = new GeometryResponse();

        geometry_stub->GeometryOperationUnary(clientContext, *operatorRequestContains, operatorResult);

        ::google::protobuf::Map< ::google::protobuf::int64, bool > stuff = operatorResult->relate_map();
//        std::string result = operatorResult->geometry_bag().geometry_strings(0);

        EXPECT_TRUE(stuff.at(0));

        auto* clientContext2 = new grpc::ClientContext();
        auto* operatorResult2 = new GeometryResponse();

        auto* operatorRequestUnion = new GeometryRequest();
        operatorRequestUnion->set_allocated_left_geometry_request(serviceOpLeft);
        operatorRequestUnion->set_allocated_right_geometry_request(serviceOpRight);
        operatorRequestUnion->set_operator_(OperatorType::UNION);
        operatorRequestUnion->mutable_operation_sr()->CopyFrom(spatialReferenceMerc);
        operatorRequestUnion->set_result_encoding(Encoding::GEOJSON);

        geometry_stub->GeometryOperationUnary(clientContext2, *operatorRequestUnion, operatorResult2);

//        fprintf(stderr, "results json %s\n", operatorResult2->geometry_bag().geojson(0).c_str());
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}