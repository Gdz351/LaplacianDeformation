#include "Deformer.h"

Deformer::Deformer(const Eigen::Matrix3Xd& V, const Eigen::Matrix3Xi& F)
{
	Eigen::Matrix3Xd cot_angles;
	cal_cot_angles(V, F, cot_angles);
	std::vector<Eigen::Triplet<double>> triple;
	triple.reserve(F.cols() * 9);
	for (size_t j = 0; j < F.cols(); ++j) {
		const Eigen::Vector3i& fv = F.col(j);
		const Eigen::Vector3d& ca = cot_angles.col(j);
		for (size_t vi = 0; vi < 3; ++vi) {
			const size_t j1 = (vi + 1) % 3;
			const size_t j2 = (vi + 2) % 3;
			const int fv0 = fv[vi];
			const int fv1 = fv[j1];
			const int fv2 = fv[j2];
			triple.push_back(Eigen::Triplet<double>(fv0, fv0, ca[j1] + ca[j2]));
			triple.push_back(Eigen::Triplet<double>(fv0, fv1, -ca[j2]));
			triple.push_back(Eigen::Triplet<double>(fv0, fv2, -ca[j1]));
		}
	}
	L.resize(V.cols(), V.cols());
	L.setFromTriplets(triple.begin(), triple.end());
}
void Deformer::cal_cot_angles(
	const Eigen::MatrixXd& V,
	const Eigen::Matrix3Xi& F,
	Eigen::Matrix3Xd& cot_angles) {
	cot_angles.resize(3, F.cols());
	for (size_t j = 0; j < F.cols(); ++j) {
		const Eigen::Vector3i& fv = F.col(j);
		for (size_t vi = 0; vi < 3; ++vi) {
			const Eigen::VectorXd& p0 = V.col(fv[vi]);
			const Eigen::VectorXd& p1 = V.col(fv[(vi + 1) % 3]);
			const Eigen::VectorXd& p2 = V.col(fv[(vi + 2) % 3]);
			const double angle = std::acos(std::max(-1.0,
				std::min(1.0, (p1 - p0).normalized().dot((p2 - p0).normalized()))));
			cot_angles(vi, j) = 1.0 / std::tan(angle);
		}
	}
}

void Deformer::update_matrix(
	const std::vector<int>& handles_id,
	const std::vector<int>& fix_id)
{
	Ls = L;
	std::vector<double*> diag(Ls.rows());
	for (int i = 0; i < diag.size(); ++i) diag[i] = &Ls.coeffRef(i, i);
	for (const auto& id : fix_id)(*diag[id]) = DOUBLE_INF;
	for (const auto& id : handles_id) (*diag[id]) = DOUBLE_INF;

	solver.analyzePattern(Ls.transpose() * Ls);
	solver.factorize(Ls.transpose() * Ls);
}
void Deformer::laplace_deformation(
	Eigen::Matrix3Xd& V, const Eigen::Matrix4d& trans,
	const std::vector<int>& handles_id,
	const std::vector<int>& unconstrain_id,
	const std::vector<int>& fix_id)
{
	Eigen::MatrixXd B = L * V.transpose();

	auto V_new = V.transpose();
	for (auto index : handles_id) {
		Eigen::Vector4d temp;
		temp << (Eigen::Vector3d)V_new.row(index), 1.0;
		temp = trans * temp;
		V_new.row(index) = temp.head<3>();
	}
	for (const auto& id : fix_id) B.row(id) = DOUBLE_INF * V_new.row(id);
	for (const auto& id : handles_id) B.row(id) = DOUBLE_INF * V_new.row(id);

	if (solver.info() != Eigen::Success)
		std::cout << "Compute failed: " << solver.info() << std::endl;
	V = solver.solve(Ls.transpose() * B).transpose();
}


