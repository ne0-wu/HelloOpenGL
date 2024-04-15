#include <Eigen/Core>
#include <Eigen/Sparse>

#include "Mesh.h"
#include "PointCloud.h"

// #define IMPLEMENT_QEM
#define IMPLEMENT_LGP
// #define IMPLEMENT_PSR

void poissonSurfaceReconstruction(PointCloud &pointCloud);

class Parameterization
{
public:
	Parameterization(Mesh const &mesh) : mesh(mesh) {}
	virtual void flatten() = 0;
	Eigen::MatrixX2d getUV() { return uv; }

protected:
	Mesh const &mesh;	 // input mesh
	Eigen::MatrixX2d uv; // output uv coordinates
};

class Tutte : public Parameterization
{
public:
	enum class LaplacianType
	{
		UNIFORM,
		COTANGENT
	};

	Tutte(Mesh const &mesh, LaplacianType laplacianType = LaplacianType::UNIFORM)
		: Parameterization(mesh), laplacianType(laplacianType) {}

	void setLaplacianType(LaplacianType type)
	{
		laplacianType = type;
	}

	void flatten();

protected:
	// Reusable variables
	std::vector<double> cotangents;
	std::vector<Mesh::VertexHandle> boundaryVertices;

	// Tutte parameterization
	void tutte(Eigen::SparseMatrix<double> const &laplacian);

	LaplacianType laplacianType;
	Eigen::SparseMatrix<double> laplacianUniform();
	Eigen::SparseMatrix<double> laplacianCotangent();

	Eigen::SparseMatrix<double> laplacian;

	// Opposite angle of a halfedge
	double oppositeAngle(Mesh::HalfedgeHandle heh) const;

	// Pre-compute cotangent
	void computeCotangents();

	// Find boundary vertices
	void findBoundaryVertices();

	void tutte();
};

class LocalGlobal : public Tutte
{
public:
	enum class LocalGlobalTarget
	{
		ARAP,
		ASAP
	};

	LocalGlobal(Mesh const &mesh, LocalGlobalTarget target = LocalGlobalTarget::ARAP, int numIter = 100)
		: Tutte(mesh), target(target), numIter(numIter) {}

	void setNumIter(int numIter) { this->numIter = numIter; }
	int getNumIter() { return numIter; }

	void flatten();

protected:
	int numIter;
	LocalGlobalTarget target;

	void localGlobal();
};