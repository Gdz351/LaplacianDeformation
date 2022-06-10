#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Cholesky>
#include <Eigen/Sparse>

#include <iostream>
#include <vector>

const double DOUBLE_INF = 1e8;

class Deformer
{
public:
	Deformer() = default;
	Deformer(const Eigen::Matrix3Xd& V, const Eigen::Matrix3Xi& F);

	void cal_cot_angles(const Eigen::MatrixXd& V, const Eigen::Matrix3Xi& F, Eigen::Matrix3Xd& cot_angles);

	void laplace_deformation(Eigen::Matrix3Xd& V, const Eigen::Matrix4d& trans,
		const std::vector<int>& handles_id, const std::vector<int>& unconstrain_id, const std::vector<int>& fix_id);
	void update_matrix(const std::vector<int>& handles_id, const std::vector<int>& fix_id);

	~Deformer() = default;
private:
	Eigen::SparseMatrix<double> L;
	Eigen::SparseMatrix<double> Ls;
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>>   solver;
};

