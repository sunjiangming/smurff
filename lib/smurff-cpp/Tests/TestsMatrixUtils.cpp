#include "catch.hpp"

#include <Eigen/Core>

#include <SmurffCpp/Configs/TensorConfig.h>
#include <SmurffCpp/Utils/MatrixUtils.h>

using namespace smurff;

TEST_CASE("matrix_tuils::slice : error handling")
{
   std::vector<std::uint64_t> tensorConfigDims = { 2, 3, 4, 2 };
   std::vector<std::uint32_t> tensorConfigColumns =
      {
         0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,
         0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,
         0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,

         0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,
         0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,
         0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1
      };
   std::vector<double> tensorConfigValues =
      {
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
        24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47
      };
   TensorConfig tensorConfig(tensorConfigDims, tensorConfigColumns, tensorConfigValues, NoiseConfig());

   // Error handling when both fixed dims values are the same
   REQUIRE_THROWS(matrix_utils::slice(tensorConfig, { 0, 0 }, {{ 1, 0 }, { 2, 1 }}));

   // Error handking where fixed dims contain invalid dim
   REQUIRE_THROWS(matrix_utils::slice(tensorConfig, { 9, 0 }, {{ 1, 0 }, { 2, 1 }}));

   // Error handling when there is a missing coord in dimCoords
   REQUIRE_THROWS(matrix_utils::slice(tensorConfig, { 0, 1 }, {{ 2, 0 }}));

   // Error handling when there is an invalid dimension in dimCoords
   REQUIRE_THROWS(matrix_utils::slice(tensorConfig, { 0, 1 }, {{ 2, 0 }, { 4, 2 }}));

   // Error handling when there is an invalid coord in dimCoords
   REQUIRE_THROWS(matrix_utils::slice(tensorConfig, { 0, 1 }, {{ 2, 100 }, { 3, 1 }}));

   // Error handling when fixed dims and dim coords intersect
   REQUIRE_THROWS(matrix_utils::slice(tensorConfig, { 0, 1 }, {{ 1, 0 }, { 3, 1 }}));
}

TEST_CASE("matrix_utils::slice : 3D tensor")
{
   std::vector<std::uint64_t> tensorConfigDims = { 2, 3, 4 };
   std::vector<std::uint32_t> tensorConfigColumns =
      {
         0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1,
         0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,  0,  0,  1,  1,  2,  2,
         0,  0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,
      };
   std::vector<double> tensorConfigValues =
      {
         0,  3,  1,  4,  2,  5,  6,  9,  7, 10,  8, 11, 12, 15, 13, 16, 14, 17, 18, 21, 19, 22, 20, 23
      };
   TensorConfig tensorConfig(tensorConfigDims, tensorConfigColumns, tensorConfigValues, NoiseConfig());

   // 1st and 2nd dimension slice. Matrix with 2 rows and 3 cols
   {
      Eigen::MatrixXd actualTensorSlice =
         matrix_utils::slice( tensorConfig
                            , { 0, 1 }
                            , {{ 2, 3 }}
                            );

      Eigen::MatrixXd expectedTensorSlice(2, 3);
      expectedTensorSlice << 18, 19, 20,
                             21, 22, 23;

      REQUIRE(matrix_utils::equals(actualTensorSlice, expectedTensorSlice));
   }

   // 2nd and 1st dimension slice. Matrix with 3 rows and 2 cols
   {
      Eigen::MatrixXd actualTensorSlice =
         matrix_utils::slice( tensorConfig
                            , { 1, 0 }
                            , {{ 2, 3 }}
                            );

      Eigen::MatrixXd expectedTensorSlice(3, 2);
      expectedTensorSlice << 18, 21,
                             19, 22,
                             20, 23;

      REQUIRE(matrix_utils::equals(actualTensorSlice, expectedTensorSlice));
   }

   // 2nd and 3rd dimension slice. Matrix with 3 rows and 4 cols
   {
      Eigen::MatrixXd actualTensorSlice =
         matrix_utils::slice( tensorConfig
                            , { 1, 2 }
                            , {{ 0, 1 }}
                            );

      Eigen::MatrixXd expectedTensorSlice(3, 4);
      expectedTensorSlice << 3,  9, 15, 21,
                             4, 10, 16, 22,
                             5, 11, 17, 23;

      REQUIRE(matrix_utils::equals(actualTensorSlice, expectedTensorSlice));
   }

   // 3rd and 2nd dimension slice. Matrix with 4 rows and 3 cols
   {
      Eigen::MatrixXd actualTensorSlice =
         matrix_utils::slice( tensorConfig
                            , { 2, 1 }
                            , {{ 0, 1 }}
                            );

      Eigen::MatrixXd expectedTensorSlice(4, 3);
      expectedTensorSlice <<  3,  4,  5,
                              9, 10, 11,
                             15, 16, 17,
                             21, 22, 23;

      REQUIRE(matrix_utils::equals(actualTensorSlice, expectedTensorSlice));
   }

   // 1st and 3rd dimension slice. Matrix with 2 rows and 4 cols
   {
      Eigen::MatrixXd actualTensorSlice =
         matrix_utils::slice( tensorConfig
                            , { 0, 2 }
                            , {{ 1, 1 }}
                            );

      Eigen::MatrixXd expectedTensorSlice(2, 4);
      expectedTensorSlice << 1,  7, 13, 19,
                             4, 10, 16, 22;

      REQUIRE(matrix_utils::equals(actualTensorSlice, expectedTensorSlice));
   }

   // 3rd and 1st dimension slice. Matrix with 4 rows and 2 cols
   {
      Eigen::MatrixXd actualTensorSlice =
         matrix_utils::slice( tensorConfig
                            , { 2, 0 }
                            , {{ 1, 1 }}
                            );

      Eigen::MatrixXd expectedTensorSlice(4, 2);
      expectedTensorSlice <<  1,  4,
                              7, 10,
                             13, 16,
                             19, 22;

      REQUIRE(matrix_utils::equals(actualTensorSlice, expectedTensorSlice));
   }
}

TEST_CASE("matrix_utils::slice : 6D tensor")
{
   // TODO
}