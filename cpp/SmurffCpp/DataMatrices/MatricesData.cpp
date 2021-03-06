#include "MatricesData.h"

#include <SmurffCpp/Utils/Error.h>

namespace smurff {

MatricesData::MatricesData()
   : total_dim(2)
{
   name = "MatricesData";
}

void MatricesData::init_pre()
{
   mode_dim.resize(nmode());
   for(std::uint64_t n = 0; n < nmode(); ++n)
   {
       std::vector<int> S(blocks.size());
       int max_pos = -1;
       for(auto &blk : blocks)
       {
           int pos  = blk.pos(n); // get coordinate of underlying matrix
           int size = blk.dim(n); // get dimension size of underlying matrix

           THROWERROR_ASSERT(size > 0);

           THROWERROR_ASSERT(S.at(pos) == 0 || S.at(pos) == size);

           S.at(pos) = size;
           max_pos = std::max(max_pos, pos);
       }
       int off = 0;
       auto &O = mode_dim.at(n);
       O.resize(max_pos+2);
       for(int pos=0; pos<=max_pos; ++pos)
       {
           O.at(pos) = off;
           off += S[pos];
       }
       O.at(max_pos+1) = off;
       total_dim.at(n) = off;
       for(auto &blk : blocks)
       {
           int pos = blk.pos(n);
           blk._start.at(n) = O[pos];
       }

   }

   // init sub-matrices
   for(auto &p : blocks)
   {
      p.data()->init_pre();
   }
}

void MatricesData::init_post()
{
   Data::init_post();

   // init sub-matrices
   for(auto &p : blocks)
   {
      p.data()->init_post();
   }
}

std::shared_ptr<Data> MatricesData::add(const PVec<>& p, std::shared_ptr<Data> data)
{
   blocks.push_back(Block(p, data));
   return blocks.back().data();
}

double MatricesData::sumsq(const SubModel& model) const
{
   THROWERROR_NOTIMPL();
}

double MatricesData::var_total() const
{
   return NAN;
}

double MatricesData::train_rmse(const SubModel& model) const
{
   double sum = .0;
   std::uint64_t N = 0;
   int count = 0;

   for(auto &p : blocks)
   {
       auto mtx = p.data();
       double local_rmse = mtx->train_rmse(p.submodel(model));
       sum += (local_rmse * local_rmse) * (mtx->size() - mtx->nna());
       N += (mtx->size() - mtx->nna());
       count++;
   }

   THROWERROR_ASSERT(N > 0);

   return std::sqrt(sum / N);
}

void MatricesData::update(const SubModel &model)
{
   for(auto &b : blocks)
   {
      b.data()->update(b.submodel(model));
   }
}

void MatricesData::getMuLambda(const SubModel& model, uint32_t mode, int pos, Vector& rr, Matrix& MM) const
{
   int count = 0;
   apply(mode, pos, [&model, mode, pos, &rr, &MM, &count](const Block &b) {
       b.data()->getMuLambda(b.submodel(model), mode, pos - b.start(mode), rr, MM);
       count++;
   });

   THROWERROR_ASSERT(count > 0);
}

void MatricesData::update_pnm(const SubModel& model, uint32_t mode)
{
   for(auto &b : blocks) {
      b.data()->update_pnm(b.submodel(model), mode);
  }
}

std::ostream& MatricesData::info(std::ostream& os, std::string indent)
{
   MatrixData::info(os, indent);
   os << indent << "Sub-Matrices:\n";
   for(auto &p : blocks)
   {
       os << indent;
       p.pos().info(os);
       os << ":\n";
       p.data()->info(os, indent + "  ");
       os << std::endl;
   }
   return os;
}

std::ostream& MatricesData::status(std::ostream& os, std::string indent) const
{
   os << indent << "Sub-Matrices:\n";
   for(auto &p : blocks)
   {
       os << indent << "  ";
       p.pos().info(os);
       os << ": " << p.data()->noise().getStatus() << "\n";
   }
   return os;
}

std::uint64_t MatricesData::nnz() const
{
   return accumulate(0LL, &MatrixData::nnz);
}

std::uint64_t MatricesData::nna() const
{
   return accumulate(0LL, &MatrixData::nna);
}

double MatricesData::sum() const
{
   return accumulate(.0, &MatrixData::sum);
}

PVec<> MatricesData::dim() const
{
   return total_dim;
}

MatricesData::Block::Block(PVec<> p, std::shared_ptr<Data> m)
   : _pos(p)
   , _start(2)
   , m_matrix(m)
{
}

const PVec<> MatricesData::Block::start() const
{
   return _start;
}

const PVec<> MatricesData::Block::end() const
{
   return start() + dim();
}

const PVec<> MatricesData::Block::dim() const
{
   return data()->dim();
}

const PVec<> MatricesData::Block::pos() const
{
   return _pos;
}

int MatricesData::Block::start(int mode) const
{
   return start().at(mode);
}

int MatricesData::Block::end(int mode) const
{
   return end().at(mode);
}

int MatricesData::Block::dim(int mode) const
{
   return dim().at(mode);
}

int MatricesData::Block::pos(int mode) const
{
   return pos().at(mode);
}

std::shared_ptr<Data> MatricesData::Block::data() const
{
   return m_matrix;
}

bool MatricesData::Block::in(const PVec<> &p) const
{
   return p.in(start(), end());
}

bool MatricesData::Block::in(int mode, int p) const
{
   return p >= start(mode) && p < end(mode);
}

SubModel MatricesData::Block::submodel(const SubModel& model) const
{
   return SubModel(model, start(), dim());
}

const MatricesData::Block& MatricesData::find(const PVec<>& p) const
{
   return *std::find_if(blocks.begin(), blocks.end(), [p](const Block &b) -> bool { return b.in(p); });
}

int MatricesData::nview(int mode) const
{
   return mode_dim.at(mode).size() - 1;
}

int MatricesData::view(int mode, int pos) const
{
   THROWERROR_ASSERT(pos < MatrixData::dim(mode));

   const auto &v = mode_dim.at(mode);
   for(int i=0; i<nview(mode); ++i)
      if (pos < v.at(i + 1))
         return i;

   THROWERROR_ASSERT(false);
}

int MatricesData::view_size(int mode, int v) const {
    const auto &M = mode_dim.at(mode);
    return M.at(v+1) - M.at(v);
}
} // end namespace smurff
