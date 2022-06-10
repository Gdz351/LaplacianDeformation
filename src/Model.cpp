#include "Model.h"
#define _USE_MATH_DEFINES
#include <math.h>

Model::Model(const std::string& path)
{
	if (!OpenMesh::IO::read_mesh(_mesh, path))
	{
		std::cerr << "read error\n";
		exit(1);
	}
	V.resize(3, _mesh.n_vertices());
	for (auto v : _mesh.vertices())
	{
		float* date = _mesh.point(v).data();
		V.col(v.idx()) << date[0], date[1], date[2];
	}

	F.resize(3, _mesh.n_faces());
	for (auto face : _mesh.faces()) {
		std::vector<int> id;
		for (auto fv : _mesh.fv_range(face)) {
			id.push_back(fv.idx());
		}
		F.col(face.idx()) << id[0], id[1], id[2];
	}

	normalized_vertex();

}

void Model::normalized_vertex() {

	Eigen::Vector3d center = V.rowwise().sum();
	Eigen::Vector3d max = V.rowwise().maxCoeff();
	Eigen::Vector3d min = V.rowwise().minCoeff();

	center /= V.cols();
	double max_len = (max - min).maxCoeff();

	V.colwise() -= center;
	V /= max_len;

}

void Model::update_vertex_state(int id, double handles_radius, double unconstrain_radius)
{
	_handles_id.clear();
	_unconstrain_id.clear();
	_fix_id.clear();

	std::vector<bool> flag(V.cols());
	std::queue<std::pair<int, double>> _queue;
	_queue.push({ id,0 });
	_handles_id.push_back(id);
	flag[id] = true;

	while (!_queue.empty()) {
		auto node = _queue.front();
		_queue.pop();

		for (const auto& adj_it : _mesh.vv_range(*(_mesh.vertices_begin() + node.first))) {
			int adj_id = adj_it.idx();
			if (flag[adj_id] == true)continue;
			double adj_dis = (V.col(node.first) - V.col(adj_id)).norm();
			if (node.second + adj_dis < handles_radius) {
				flag[adj_id] = true;
				_queue.push({ adj_id,node.second + adj_dis });
				_handles_id.push_back(adj_id);
			}
			else if (node.second + adj_dis < (handles_radius + unconstrain_radius)) {
				flag[adj_id] = true;
				_queue.push({ adj_id,node.second + adj_dis });
				_unconstrain_id.push_back(adj_id);
			}
		}
	}

	for (int i = 0; i < flag.size(); ++i) {
		if (!flag[i])_fix_id.push_back(i);
	}
}

