#pragma once

#include <vector>
#include <iostream>
#include <memory>

#include "TensorConfig.h"
#include "NoiseConfig.h"

namespace smurff
{
   class Data;

   class MatrixConfig : public TensorConfig
   {
   private:
      mutable std::vector<std::uint32_t> m_rows;
      mutable std::vector<std::uint32_t> m_cols;

   //
   // Dense double matrix constructos
   //
   public:
      MatrixConfig(std::uint64_t nrow, std::uint64_t ncol,
                   const double* values,
                   const NoiseConfig& noiseConfig);

   //
   // Sparse double matrix constructors
   //
   public:
      MatrixConfig(std::uint64_t nrow, std::uint64_t ncol,
                   const std::uint32_t* rows, const std::uint32_t* cols, const double* values,
                   const NoiseConfig& noiseConfig, bool isScarce);

   //
   // Sparse binary matrix constructors
   //
   public:
      MatrixConfig(std::uint64_t nrow, std::uint64_t ncol,
                   const std::uint32_t* rows, const std::uint32_t* cols,
                   const NoiseConfig& noiseConfig, bool isScarce);

   //
   // Constructors for constructing sparse matrix as a tensor
   //
   public:
      MatrixConfig(std::uint64_t nrow, std::uint64_t ncol,
                   const std::uint32_t* columns, const double* values,
                   const NoiseConfig& noiseConfig, bool isScarce);

   //
   // Constructors for constructing sparse binary matrix as a tensor
   //
   public:
      MatrixConfig(std::uint64_t nrow, std::uint64_t ncol,
                   const std::uint32_t* columns,
                   const NoiseConfig& noiseConfig, bool isScarce);

   public:
      MatrixConfig();

   public:
      std::uint64_t getNRow() const;
      std::uint64_t getNCol() const;

      const std::vector<std::uint32_t>& getRows() const;
      const std::vector<std::uint32_t>& getCols() const;

   public:
      std::shared_ptr<Data> create(std::shared_ptr<IDataCreator> creator) const override;

   public:
      void write(std::shared_ptr<IDataWriter> writer) const override;
   };
}
