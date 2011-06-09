#include "GmshConfig.h"
#include "meshGRegionMMG3D.h"
#ifdef HAVE_MMG3D
#include <set>
#include "GRegion.h"
#include "GFace.h"
#include "MTetrahedron.h"
#include "MTriangle.h"
#include "MVertex.h"
#include "BackgroundMesh.h"
#include "Context.h"

extern "C" {
#include <libmmg3d.h>
#define M_UNUSED    (1 << 0)
}

void MMG2gmsh (GRegion *gr, MMG_pMesh mmg, std::map<int,MVertex*> &mmg2gmsh){
  printf("%d vertices in MMG\n",mmg->np);
  for (int k=1;k<= mmg->np ; k++){
    MMG_pPoint ppt = &mmg->point[k]; 
    if (ppt->tag & M_UNUSED) continue;
    if (mmg2gmsh.find(k) == mmg2gmsh.end()){
      MVertex *v = new MVertex(ppt->c[0],ppt->c[1],ppt->c[2],gr);
      mmg2gmsh[k] = v;
      gr->mesh_vertices.push_back(v);
    }
  }  

  
  for (int k=1; k<=mmg->ne; k++) { 
    MMG_pTetra ptetra = &mmg->tetra[k]; 
    if ( ptetra->v[0] ){
      MVertex *v1 = mmg2gmsh[ptetra->v[0]];
      MVertex *v2 = mmg2gmsh[ptetra->v[1]];
      MVertex *v3 = mmg2gmsh[ptetra->v[2]];
      MVertex *v4 = mmg2gmsh[ptetra->v[3]];
      if (!v1 || !v2 || !v3 || !v4){
	Msg::Error("Element %d Unknown Vertex in MMG2gmsh %d(%p) %d(%p) %d(%p) %d(%p)",k,ptetra->v[0],v1,ptetra->v[1],v2,ptetra->v[2],v3,ptetra->v[3],v4);
      }
      else gr->tetrahedra.push_back(new MTetrahedron(v1,v2,v3,v4));
    }
  }
}

void gmsh2MMG (GRegion *gr, MMG_pMesh mmg, MMG_pSol sol, std::map<int,MVertex*> &mmg2gmsh){
  mmg->ne = gr->tetrahedra.size();
  std::set<MVertex*> allVertices;
  for (int i=0;i< gr->tetrahedra.size() ; i++){
    allVertices.insert(gr->tetrahedra[i]->getVertex(0));
    allVertices.insert(gr->tetrahedra[i]->getVertex(1));
    allVertices.insert(gr->tetrahedra[i]->getVertex(2));
    allVertices.insert(gr->tetrahedra[i]->getVertex(3));
  }
  mmg->np = sol->np = allVertices.size();
  
  std::list<GFace*> f = gr->faces();

  mmg->nt = 0;
  for (std::list<GFace*>::iterator it = f.begin(); it != f.end() ; ++it){
    mmg->nt += (*it)->triangles.size();
  }
  
  mmg->npmax = sol->npmax = 1000000;
  mmg->ntmax = 700000;
  mmg->nemax = 7000000;

  mmg->point = (MMG_pPoint)calloc(mmg->npmax+1,sizeof(MMG_Point));
  mmg->tetra = (MMG_pTetra)calloc(mmg->nemax+1,sizeof(MMG_Tetra));
  mmg->tria  = (MMG_pTria) calloc(mmg->ntmax+1,sizeof(MMG_Tria));
  mmg->disp  = (MMG_pDispl)calloc(mmg->npmax+1,sizeof(MMG_Displ));
  mmg->adja = (int*)calloc(4*mmg->nemax+5,sizeof(int));

  sol->offset = 6;
  sol->met = (double*)calloc(sol->npmax+1,sol->offset*sizeof(double)); 
  sol->metold = (double*)calloc(sol->npmax+1,sol->offset*sizeof(double)); 

  std::map<MVertex*,std::pair<double,int> > LCS;
  if (CTX::instance()->mesh.lcExtendFromBoundary){
    for (std::list<GFace*>::iterator it = f.begin(); it != f.end() ; ++it){
      for (int i=0;i<(*it)->triangles.size();i++){
	MTriangle *t = (*it)->triangles[i];
	double L = t->maxEdge();
	for (int k=0;k<3;k++){
	  MVertex *v = t->getVertex(k);
	  std::map<MVertex*,std::pair<double,int> >::iterator itv = LCS.find(v);
	  if (itv != LCS.end()){
	    itv->second.first += L;
	    itv->second.second ++;
	  }
	  else {
	    LCS[v] = std::make_pair(L,1);
	  }
	}
      }
    }
  }

  
  int k=1;
  int count = 1;//sol->offset;
  std::map<int,int> gmsh2mmg_num;
  for (std::set<MVertex*>::iterator it = allVertices.begin() ; it != allVertices.end() ; ++it){
    MMG_pPoint ppt = &mmg->point[k]; 

    mmg2gmsh[k] = *it;

    ppt->c[0] = (*it)->x();
    ppt->c[1] = (*it)->y();
    ppt->c[2] = (*it)->z();
    ppt->ref  = gr->tag();
    gmsh2mmg_num[(*it)->getNum()] = k;

    MVertex *v = *it;
    double U=0,V=0;
    if (v->onWhat()->dim() == 1){
      v->getParameter(0,U);
    }
    else if (v->onWhat()->dim() == 2){
      v->getParameter(0,U);
      v->getParameter(1,V);
    }
    double lc = BGM_MeshSize(v->onWhat(), U,V,v->x(), v->y(), v->z());  
    //SMetric3 m = BGM_MeshMetric(v->onWhat(), U,V,v->x(), v->y(), v->z());  

    if (CTX::instance()->mesh.lcExtendFromBoundary){
      std::map<MVertex*,std::pair<double,int> >::iterator itv = LCS.find(v);
      if (itv != LCS.end()){
	double LL = itv->second.first/itv->second.second;
	//	SMetric3 l4(1./(LL*LL));
	//	SMetric3 MM = intersection (l4, m);	
	//	m = MM;
	lc = std::min(LL,lc);
      }
    }

    sol->met[count++] = 1/(lc*lc);
    sol->met[count++] = 0;
    sol->met[count++] = 0;
    sol->met[count++] = 1/(lc*lc);
    sol->met[count++] = 0;
    sol->met[count++] = 1/(lc*lc);
    //    printf("%g %g %g %g %g %g\n",m(0,0),m(0,1),m(0,2),m(1,1),m(1,2),m(2,2));
    
    //    for (int i=0; i<sol->offset; i++)  {
    //      sol->met[isol + i] = lc;
      //      printf("sol[%d] = %12.5E\n",isol + i,lc);
    //    }
    k++;
  }
  
  for (k=1; k<=mmg->ne; k++) { 
    MMG_pTetra ptetra = &mmg->tetra[k]; 
    ptetra->v[0] = gmsh2mmg_num[gr->tetrahedra[k-1]->getVertex(0)->getNum()];
    ptetra->v[1] = gmsh2mmg_num[gr->tetrahedra[k-1]->getVertex(1)->getNum()];
    ptetra->v[2] = gmsh2mmg_num[gr->tetrahedra[k-1]->getVertex(2)->getNum()];
    ptetra->v[3] = gmsh2mmg_num[gr->tetrahedra[k-1]->getVertex(3)->getNum()];
    ptetra->ref  = gr->tag();
  }

  k = 1;
  for (std::list<GFace*>::iterator it = f.begin(); it != f.end() ; ++it){
    for (int i=0;i<(*it)->triangles.size();i++){
      MMG_pTria ptriangle = &mmg->tria[k]; 
      ptriangle->v[0] = gmsh2mmg_num[(*it)->triangles[i]->getVertex(0)->getNum()];
      ptriangle->v[1] = gmsh2mmg_num[(*it)->triangles[i]->getVertex(1)->getNum()];
      ptriangle->v[2] = gmsh2mmg_num[(*it)->triangles[i]->getVertex(2)->getNum()];
      ptriangle->ref  = (*it)->tag();
      k++;
    }
  } 
  mmg->disp = 0;
  
}

void refineMeshMMG(GRegion *gr){
  MMG_pMesh mmg = (MMG_pMesh)calloc(1,sizeof(MMG_Mesh)); 
  MMG_pSol  sol = (MMG_pSol)calloc(1,sizeof(MMG_Sol)); 
  std::map<int,MVertex*> mmg2gmsh;
  gmsh2MMG (gr, mmg, sol,mmg2gmsh);
  int opt[9] = {1,0,64,0,0,(Msg::GetVerbosity() > 20) ? 3 : -1,0,0,0};
  mmg3d::MMG_mmg3dlib(opt,mmg,sol); 

  for (int i=0;i<gr->tetrahedra.size();++i)delete gr->tetrahedra[i];
  gr->tetrahedra.clear();
  for (int i=0;i<gr->mesh_vertices.size();++i)delete gr->mesh_vertices[i];
  gr->mesh_vertices.clear();


  MMG2gmsh (gr, mmg, mmg2gmsh);  
  MMG_saveMesh(mmg ,"test.mesh");
}

#else
void refineMeshMMG(GRegion *gr){
  Msg::Error("You should compile your version of Gmsh with MMG3D, the Mobile Mesh Generator");
}
#endif
