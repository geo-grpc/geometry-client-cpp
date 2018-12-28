//
// Created by David Raleigh on 3/7/18.
//

//#include "../geometry/geometry_operators.grpc.pb.h"
//#include "../geometry/geometry_operators.pb.h"
#include <geometry_operators.pb.h>
#include <geometry_operators.grpc.pb.h>
#include "gtest/gtest.h"
#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <cstdlib>
#include <memory>


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
        std::unique_ptr<epl::geometry::GeometryOperators::Stub> geometry_stub = epl::geometry::GeometryOperators::NewStub(channel);

        epl::geometry::GeometryBagData serviceGeometry;
        const char* wkt = "MULTIPOLYGON (((40 40, 20 45, 45 30, 40 40)), ((20 35, 45 20, 30 5, 10 10, 10 30, 20 35), (30 20, 20 25, 20 15, 30 20)))";
        serviceGeometry.add_wkt(wkt);
        serviceGeometry.set_geometry_encoding_type(epl::geometry::GeometryEncodingType::wkt);

        epl::geometry::GeometryBagData cutterGeometry;
        const char* wkt_cutter = "LINESTRING(0 0, 45 45)";
        cutterGeometry.add_wkt(wkt_cutter);
        cutterGeometry.set_geometry_encoding_type(epl::geometry::GeometryEncodingType::wkt);

        auto* operatorRequest = new epl::geometry::OperatorRequest();
        operatorRequest->mutable_left_geometry_bag()->CopyFrom(serviceGeometry);
        operatorRequest->mutable_right_geometry_bag()->CopyFrom(cutterGeometry);
        operatorRequest->set_operator_type(epl::geometry::ServiceOperatorType::Cut);
        operatorRequest->set_results_encoding_type(epl::geometry::GeometryEncodingType::wkt);

        auto* clientContext = new grpc::ClientContext();
        auto* operatorResult = new epl::geometry::OperatorResult();

        geometry_stub->ExecuteOperation(clientContext, *operatorRequest, operatorResult);

        std::string result1 = operatorResult->geometry_bag().wkt(0);
        std::string result2 = operatorResult->geometry_bag().wkt(1);
        const char* expected1 = "MULTIPOLYGON (((35.625 35.625, 40 40, 20 45, 35.625 35.625)), ((10 10, 20 20, 20 25, 23.333333333333336 23.333333333333336, 29.375 29.375, 20 35, 10 30, 10 10)))";
        const char* expected2 = "MULTIPOLYGON (((40 40, 35.625 35.625, 45 30, 40 40)), ((30 5, 45 20, 29.375 29.375, 23.333333333333336 23.333333333333336, 30 20, 20 15, 20 20, 10 10, 30 5)))";

        EXPECT_STREQ(expected1 , result1.c_str());
        EXPECT_STREQ(expected2 , result2.c_str());
    }

    TEST_F(GeometryClientTest, TEST_2) {
        std::shared_ptr<grpc::Channel> channel;
        if (const char* env_p = std::getenv("GEOMETRY_SERVICE_HOST")) {
            channel = grpc::CreateChannel(env_p, grpc::InsecureChannelCredentials());
        } else {
            channel = grpc::CreateChannel("localhost:8980", grpc::InsecureChannelCredentials());
        }
        std::unique_ptr<epl::geometry::GeometryOperators::Stub> geometry_stub = epl::geometry::GeometryOperators::NewStub(channel);

        epl::geometry::SpatialReferenceData spatialReferenceWGS84;
        spatialReferenceWGS84.set_wkid(4326);

        auto* spatialReferenceCalif = new epl::geometry::SpatialReferenceData();
        spatialReferenceCalif->set_wkid(32632);

        // allocating this here means it is not copied in the set_allocated method, but a strange rule of control is given to the
        // operator request message
        auto* serviceGeometry = new epl::geometry::GeometryBagData();
        const char* wkt = "MULTILINESTRING ((500000       0, 400000  100000, 600000 -100000))";
        serviceGeometry->add_wkt(wkt);
        serviceGeometry->set_geometry_encoding_type(epl::geometry::GeometryEncodingType::wkt);
        serviceGeometry->set_allocated_spatial_reference(spatialReferenceCalif);

        auto* operatorRequest = new epl::geometry::OperatorRequest();
        operatorRequest->mutable_result_spatial_reference()->CopyFrom(spatialReferenceWGS84);
        operatorRequest->set_allocated_left_geometry_bag(serviceGeometry);
        operatorRequest->set_allocated_operation_spatial_reference(spatialReferenceCalif);
        operatorRequest->set_operator_type(epl::geometry::ServiceOperatorType::Project);
        operatorRequest->set_results_encoding_type(epl::geometry::GeometryEncodingType::wkt);

        auto* clientContext = new grpc::ClientContext();
        auto* operatorResult = new epl::geometry::OperatorResult();

        geometry_stub->ExecuteOperation(clientContext, *operatorRequest, operatorResult);

        std::string result = operatorResult->geometry_bag().wkt(0);

        const char* expected = "MULTILINESTRING ((9 0, 8.101251062924646 0.904618578893133, 9.898748937075354 -0.904618578893133))";
        EXPECT_STREQ(expected , result.c_str());
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}