#pragma once
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <Eigen/Cholesky>
# include<Eigen/Sparse>
#include <OpenMesh/Core/IO/MeshIO.hh> 
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh> 

#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <iostream>


class Model
{
public:
	Model(const std::string& path);
	void update_vertex_state(int id, double handles_radius, double unconstrain_radius);
private:
	void normalized_vertex();

public:
	typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;
	MyMesh _mesh;
	Eigen::Matrix3Xd V;
	Eigen::Matrix3Xi F;

	std::vector<int> _handles_id;
	std::vector<int> _unconstrain_id;
	std::vector<int> _fix_id;
};

