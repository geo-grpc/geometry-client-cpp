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

    TEST_F(GeometryClientTest, TEST_2) {
        std::shared_ptr<grpc::Channel> channel;
        if (const char* env_p = std::getenv("GEOMETRY_SERVICE_HOST")) {
            channel = grpc::CreateChannel(env_p, grpc::InsecureChannelCredentials());
        } else {
            channel = grpc::CreateChannel("localhost:8980", grpc::InsecureChannelCredentials());
        }

        std::unique_ptr<geometry::GeometryOperators::Stub> geometry_stub = geometry::GeometryOperators::NewStub(channel);
        geometry::SpatialReferenceData spatialReferenceWGS84;
        spatialReferenceWGS84.set_wkid(4326);

        auto* spatialReferenceCalif = new geometry::SpatialReferenceData();
        spatialReferenceCalif->set_wkid(32632);

        // allocating this here means it is not copied in the set_allocated method, but a strange rule of control is given to the
        // operator request message
        auto* serviceGeometry = new geometry::GeometryBagData();
        const char* wkt = "MULTILINESTRING ((500000       0, 400000  100000, 600000 -100000))";
        serviceGeometry->add_geometry_strings(wkt);
        serviceGeometry->set_geometry_encoding_type(geometry::GeometryEncodingType::wkt);
        serviceGeometry->set_allocated_spatial_reference(spatialReferenceCalif);

        auto* operatorRequest = new geometry::OperatorRequest();
        operatorRequest->mutable_result_spatial_reference()->CopyFrom(spatialReferenceWGS84);
        operatorRequest->set_allocated_left_geometry_bag(serviceGeometry);
        operatorRequest->set_allocated_operation_spatial_reference(spatialReferenceCalif);
        operatorRequest->set_operator_type(geometry::ServiceOperatorType::Project);
        operatorRequest->set_results_encoding_type(geometry::GeometryEncodingType::wkt);

        auto* clientContext = new grpc::ClientContext();
        auto* operatorResult = new geometry::OperatorResult();

        geometry_stub->ExecuteOperation(clientContext, *operatorRequest, operatorResult);

        std::string result = operatorResult->geometry_bag().geometry_strings(0);

        const char* expected = "MULTILINESTRING ((9 0, 8.101251062924646 0.904618578893133, 9.898748937075354 -0.904618578893133))";
        EXPECT_STREQ(expected , result.c_str());
    }
};

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}