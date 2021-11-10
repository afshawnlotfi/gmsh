#include <math.h>
#include <vector>
#include <stack>
#include "alphaShapes.h"
#include "gmsh.h"
#include <iostream>

double alphaShape (const size_t *t, const std::vector<double> &p ){
  double tetcircumcenter(double a[3], double b[3], double c[3], double d[3],
			 double circumcenter[3], double *xi, double *eta,
			 double *zeta);
  double C[3], xi, eta, zeta;
  const double *x = &p[3*t[0]];
  const double *y = &p[3*t[1]];
  const double *z = &p[3*t[2]];
  const double *w = &p[3*t[3]];
  double a[3] = {x[0], x[1], x[2]};
  double b[3] = {y[0], y[1], y[2]};
  double c[3] = {z[0], z[1], z[2]};
  double d[3] = {w[0], w[1], w[2]};
  tetcircumcenter(a,b,c,d,C, &xi, &eta, &zeta);
  double R = sqrt ((x[0]-C[0])*(x[0]-C[0])+(x[1]-C[1])*(x[1]-C[1])+(x[2]-C[2])*(x[2]-C[2]));
  std::cout << "circumRadius : " << R << "\n";
  return R;
}

//double meanEdgeLength()


static int _faces [4][3] = {{0,1,2}, {0,1,3}, {0,2,3}, {1,2,3}};

void getOrderedFace (const size_t *t, int i, size_t *f){
  size_t no1 = t[_faces[i][0]];
  size_t no2 = t[_faces[i][1]];
  size_t no3 = t[_faces[i][2]];
  size_t lo, hi, sto;
  
  if (no1>no2) {   
      sto=no1;    
      lo=no2;   
   } else {
      sto=no2;  
      lo=no1;  
   } 
   if (sto>no3) { 
      hi=sto;    
      if(lo>no3){         
	sto=lo;                
	lo=no3;
      }else {
	sto=no3;      
      }         
   }else hi=no3; 
   
   f[0] = lo;
   f[1] = sto;
   f[2] = hi;
}

int compareFourInt (const void *a , const void *b){
  const size_t *f0 = (size_t*)a;
  const size_t *f1 = (size_t*)b;
  if (f0[0] < f1[0])return 1; 
  if (f0[0] > f1[0])return -1; 
  if (f0[1] < f1[1])return 1; 
  if (f0[1] > f1[1])return -1; 
  if (f0[2] < f1[2])return 1; 
  if (f0[2] > f1[2])return -1;
  return 0;
}

int computeTetNeighbors_ (const std::vector<size_t> &tetrahedra, std::vector<size_t> &neigh){
  
  neigh.resize(tetrahedra.size());
  for (size_t i=0;i<neigh.size();i++)neigh[i] = tetrahedra.size();
  
  size_t *temp = new size_t [5*tetrahedra.size()];
  size_t counter = 0;
  for (size_t i = 0; i < tetrahedra.size(); i+=4){
    const size_t *t = &tetrahedra[i];
    for (int j=0;j<4;j++){
      size_t f[3];
      getOrderedFace ( t, j, f);
      temp[counter++] = f[0];
      temp[counter++] = f[1];
      temp[counter++] = f[2];
      temp[counter++] = i/4;      
      temp[counter++] = j;
    }
  }  

  qsort(temp, tetrahedra.size(), 5*sizeof(size_t),compareFourInt);

  // loop over faces
  counter  = 0;
  while (1){
    if (counter == tetrahedra.size())break;
    size_t *ft0 = &temp[5*(counter++)];
    if (counter == tetrahedra.size())break;
    size_t *ft1 = &temp[5*counter];

    if (compareFourInt(ft0,ft1) == 0){
      std::cout << "are equal!! \n";
      neigh[4*ft0[3]+ft0[4]] = ft1[3];
      neigh[4*ft1[3]+ft1[4]] = ft0[3];
      counter++;
    }
  }
  
  std::cout << "size of neigh : " << neigh.size() << "\n";
  for (int i=0; i<neigh.size(); i++){
    std::cout << neigh[i] << "\n";
  }

  delete [] temp;  
  return 0;
}

int alphaShapes_ (const double threshold,
		 const std::vector<double> &pts,
		 std::vector<size_t> &tetrahedra, 
		 std::vector<std::vector<size_t> > &domains,
		 std::vector<std::vector<size_t> > &boundaries,
		 std::vector<size_t> &neigh) {

  gmsh::model::mesh::tetrahedralize(pts, tetrahedra);
  std::cout << tetrahedra.size() << "\n";
  for (size_t i = 0; i < tetrahedra.size(); i++)tetrahedra[i]--;
  std::cout << "hereree \n";
  computeTetNeighbors_ (tetrahedra, neigh);

  std::vector<bool> _touched;
  _touched.resize(tetrahedra.size()/4);
  for (size_t i=0;i<_touched.size();i++)_touched[i] = false;

  for (size_t i = 0; i < tetrahedra.size(); i+=4){
      size_t *t = &tetrahedra[i];
      std::cout << "here \n";
      if (alphaShape(t, pts) < threshold && _touched[i/4] == false){
        std::stack<size_t> _s;
        std::vector<size_t> _domain;
        std::vector<size_t> _boundary;
        _s.push(i/4);
        _touched[i] = true;
        _domain.push_back(i/4);
        while(!_s.empty()){
          size_t t = _s.top();
          _s.pop();
          for (int j=0;j<4;j++){
            size_t tj = neigh[4*t+j];
            std::cout << "tj : " << tj << "\n";
            if (tj == tetrahedra.size()){
              _boundary.push_back(t);
              _boundary.push_back(j);
            }
            else if (!_touched[tj]){
              if (alphaShape(&tetrahedra[4*tj], pts) < threshold){
                _s.push(tj);
                _touched[tj] = true;
                _domain.push_back(tj);	    
              }	    
              else {
                _boundary.push_back(t);
                _boundary.push_back(j);	      
              }
            }
          }	  
        }
        boundaries.push_back(_boundary);
        domains.push_back(_domain);
      }
  }
  for (size_t i = 0; i < tetrahedra.size(); i++)tetrahedra[i]++;
  return 0;
}

