#include "GUA_OM.h"

namespace OMT
{
	/*======================================================================*/
	Model::Model()
	{
		request_vertex_status();
		request_edge_status();
		request_face_status();
	}
	Model::~Model()
	{
		release_vertex_status();
		release_edge_status();
		release_face_status();
	}
}
/*======================================================================*/
namespace OMP
{
	Model::Model()
	{
		Mesh.request_vertex_status();
		Mesh.request_edge_status();
		Mesh.request_face_status();
	}
	Model::~Model()
	{
		Mesh.release_vertex_status();
		Mesh.release_edge_status();
		Mesh.release_face_status();
	}
	/*======================================================================*/
	bool Model::ReadFile(std::string _fileName)
	{
		bool isRead = false;
		OpenMesh::IO::Options opt;
		if ( OpenMesh::IO::read_mesh(Mesh, _fileName, opt) )
		{
			//read mesh from filename OK!
			isRead = true;
		}
		if(isRead)
		{
			// If the file did not provide vertex normals and mesh has vertex normal ,then calculate them
			if (!opt.check( OpenMesh::IO::Options::VertexNormal ) && Mesh.has_vertex_normals())
			{
				Mesh.update_normals();
			}
		}
		return isRead;
	}
	bool Model::SaveFile(std::string _fileName)
	{
		bool isSave = false;
		OpenMesh::IO::Options opt;
		if ( OpenMesh::IO::write_mesh(Mesh, _fileName, opt) )
		{
			//read mesh from filename OK!
			isSave = true;
		}
		return isSave;
	}
	/*======================================================================*/
	void Model::Render_solid()
	{
		FIter f_it;
		FVIter	fv_it;
		glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(2.0, 2.0);
		glBegin(GL_POLYGON);
		//glColor4f(1.0, 0.5, 1.0, 0.5);
		for (f_it = Mesh.faces_begin(); f_it != Mesh.faces_end(); ++f_it) 
		{
			for (fv_it = Mesh.fv_iter( f_it ); fv_it; ++fv_it)
			{						
				glNormal3dv(Mesh.normal(fv_it.handle()).data());
				glVertex3dv(Mesh.point(fv_it.handle()).data());
			}
		}
		glEnd();		
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::Render_wireframe()
	{
		MyMesh::HalfedgeHandle _hedge;
		EIter e_it=Mesh.edges_begin();

		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glColor3f(0.0, 0.0, 0.0);
		glLineWidth(1);
		glBegin(GL_LINES);
		for(e_it=Mesh.edges_begin(); e_it != Mesh.edges_end(); ++e_it)
		{
			_hedge = Mesh.halfedge_handle(e_it.handle(),1);

			glVertex3dv(Mesh.point(Mesh.from_vertex_handle(_hedge)).data());
			glVertex3dv(Mesh.point(Mesh.to_vertex_handle(_hedge)).data());			
		}
		glEnd();
		glEnable(GL_LIGHTING);
	}
	/*======================================================================*/
	void Model::RenderSpecifiedPoint()
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		vector<sp_p>::iterator p_itr = sp_p_list.begin();
		for (p_itr; p_itr != sp_p_list.end(); ++p_itr)
		{
			glColor3f(p_itr->r, p_itr->g, p_itr->b);
			glVertex3dv(p_itr->pt.data());
		}
		glEnd();
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::RenderSpecifiedVertex()
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		vector< sp_v >::iterator v_itr = sp_v_list.begin();
		for (v_itr; v_itr != sp_v_list.end(); ++v_itr)
		{
			glColor3f(v_itr->r, v_itr->g, v_itr->b);
			glVertex3dv(Mesh.point(v_itr->vh).data());
		}
		glEnd();
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
	void Model::RenderSpecifiedFace()
	{
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5, 1.0);
		glDisable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
		FVIter fv_itr;
		vector< sp_f >::iterator f_itr;
		for (f_itr = sp_f_list.begin(); f_itr != sp_f_list.end(); ++f_itr)
		{
			glColor3f(f_itr->r, f_itr->g, f_itr->b);
			for (fv_itr=Mesh.fv_iter(f_itr->fh); fv_itr; ++fv_itr)
			{						
				glNormal3dv(Mesh.normal(fv_itr.handle()).data());
				glVertex3dv(Mesh.point(fv_itr.handle()).data());
			}
		}
		glEnd();		
		glEnable(GL_LIGHTING);
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT,GL_FILL);
		glEnable(GL_CULL_FACE);
	}

	/*======================================================================*/
	void Model::add_sp_p(Point   _p, float _r, float _g, float _b)
	{
		sp_p input_data;
		input_data.pt = _p;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_p_list.push_back(input_data);
	}
	void Model::add_sp_v(VHandle _v, float _r, float _g, float _b)
	{
		sp_v input_data;
		input_data.vh = _v;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_v_list.push_back(input_data);
	}
	void Model::add_sp_f(FHandle _f, float _r, float _g, float _b)
	{
		sp_f input_data;
		input_data.fh = _f;
		input_data.r = _r;
		input_data.g = _g;
		input_data.b = _b;
		sp_f_list.push_back(input_data);
	}
	void Model::clear_sp_p()
	{
		sp_p_list.clear();
	}
	void Model::clear_sp_v()
	{
		sp_v_list.clear();
	}
	void Model::clear_sp_f()
	{
		sp_f_list.clear();
	}
	/*======================================================================*/
	VHandle Model::addVertex(Point _p)
	{
		int find_result = findVertex(_p);
		if (find_result != -1)
		{
			return Mesh.vertex_handle(find_result);
		}
		else
		{
			return Mesh.add_vertex(_p);
		}
	}
	FHandle Model::addFace(VHandle _v0, VHandle _v1, VHandle _v2, VHandle _v3)
	{
		vector<VHandle> face_vhandles;

		face_vhandles.clear();
		face_vhandles.push_back(_v0);
		face_vhandles.push_back(_v1);
		face_vhandles.push_back(_v2);
		face_vhandles.push_back(_v3);
		return Mesh.add_face(face_vhandles);
	}
	void Model::deleteFace(FHandle _f)
	{
		Mesh.delete_face(_f);
		Mesh.garbage_collection();
	}
	void Model::deleteFace(VHandle _v0, VHandle _v1, VHandle _v2, VHandle _v3)
	{
		/* 
		v1				v0
		*<--------------*
		|				|
		|				|
		|				|
		|		f		|
		|				|
		|				|
		|				|
		* --------------*
		v2				v3
		*/

		HEHandle v0v1 = Mesh.find_halfedge(_v0, _v1);
		if (v0v1.is_valid())
		{
			FHandle fh = Mesh.face_handle(v0v1);
			if (fh.is_valid())
			{
				Mesh.delete_face(fh);
				Mesh.garbage_collection();
			}
		}
	}
	/*======================================================================*/
	bool Model::IsVertexVertex( VHandle _vj, VHandle _vi)
	{
		for( VVIter vvit = Mesh.vv_iter(_vi) ; vvit ; ++vvit )
			if( vvit.handle() == _vj )
				return true;
		return false;
	}
	/*======================================================================*/
	int Model::quad_subdivision1(int _face_id)
	{
		/*----------------------------------------------------------------------*/
		//這段是為了解決index問題
		VHandle vq, vw, vt, vr;
		vq	 = addVertex(Point(0, 0, 100));
		vw	 = addVertex(Point(1, 0, 100));
		vt	 = addVertex(Point(1, 1, 100));
		vr	 = addVertex(Point(0, 1, 100));
		addFace(vq, vw, vt, vr);
		/*----------------------------------------------------------------------*/
		/*收集需要subdivision的face*/
		//初始table
		bool *table = new bool[Mesh.n_faces()];
		for (unsigned i = 0; i < Mesh.n_faces(); i++)
		{
			table[i] = false;
		}

		vector< FHandle > candidate_faces, last_found_face;
		last_found_face.push_back(Mesh.face_handle(_face_id));
		table[_face_id] = true;

		while(last_found_face.size() != 0)
		{
			vector< FHandle > new_found_faces;
			for (vector< FHandle >::iterator crnt_f = last_found_face.begin(); crnt_f != last_found_face.end(); ++crnt_f)
			{
				for (FFIter ff_itr = Mesh.ff_iter(*crnt_f); ff_itr; ++ff_itr)
				{
					if (table[ff_itr.handle().idx()] != true)
					{
						new_found_faces.push_back(ff_itr.handle());
						table[ff_itr.handle().idx()] = true;
					}
				}
			}
			for (vector< FHandle >::iterator f_itr = last_found_face.begin(); f_itr != last_found_face.end(); ++f_itr)
			{
				candidate_faces.push_back(*f_itr);
			}
			last_found_face.assign(new_found_faces.begin(), new_found_faces.end());
		}
		delete table;
		/*----------------------------------------------------------------------*/
		/*對candidate faces做subdivision*/
		/*
			v0		vd		v3
			*-------*-------*
			|		|		|	 
			|		|		|
			|	  ve|		|
		  va*-------*-------*vc
			|		|		|
			|		|		|
			|		|		|
			*-------*-------*
			v1		vb		v2		
		*/
		for (vector< FHandle >::iterator face_itr = candidate_faces.begin(); face_itr != candidate_faces.end(); ++face_itr)
		{
			VHandle v[4], va, vb, vc, vd, ve;
			FVIter fv_itr = Mesh.fv_iter(*face_itr);
			for (int i = 0; fv_itr; ++fv_itr)
			{
				v[i++] = fv_itr.handle();
			}

			deleteFace(v[0], v[1], v[2], v[3]);
			va	 = addVertex((Mesh.point(v[0])+Mesh.point(v[1]))/2);
			vb	 = addVertex((Mesh.point(v[1])+Mesh.point(v[2]))/2);
			vc	 = addVertex((Mesh.point(v[2])+Mesh.point(v[3]))/2);
			vd	 = addVertex((Mesh.point(v[3])+Mesh.point(v[0]))/2);
			ve	 = addVertex((Mesh.point(v[0])+Mesh.point(v[1])+Mesh.point(v[2])+Mesh.point(v[3]))/4);
			addFace(vd, v[0], va, ve);
			addFace(va, v[1], vb, ve);
			addFace(vb, v[2], vc, ve);
			addFace(vc, v[3], vd, ve);
		}
		/*----------------------------------------------------------------------*/
		deleteFace(vq, vw, vt, vr);//這行只是為了解決index問題
		/*----------------------------------------------------------------------*/
		return 0;
	}
	int Model::quad_subdivision2(int _face_id)
	{
		/*----------------------------------------------------------------------*/
		//這段是為了解決index問題
		VHandle vq, vw, vt, vr;
		vq	 = addVertex(Point(0, 0, 100));
		vw	 = addVertex(Point(1, 0, 100));
		vt	 = addVertex(Point(1, 1, 100));
		vr	 = addVertex(Point(0, 1, 100));
		addFace(vq, vw, vt, vr);
		/*----------------------------------------------------------------------*/
		/*收集需要subdivision的face*/
		//初始table
		bool *table = new bool[Mesh.n_faces()];
		for (unsigned i = 0; i < Mesh.n_faces(); i++)
		{
			table[i] = false;
		}

		vector< FHandle > candidate_faces, last_found_face;
		last_found_face.push_back(Mesh.face_handle(_face_id));
		table[_face_id] = true;

		while(last_found_face.size() != 0)
		{
			vector< FHandle > new_found_faces;
			for (vector< FHandle >::iterator crnt_f = last_found_face.begin(); crnt_f != last_found_face.end(); ++crnt_f)
			{
				for (FFIter ff_itr = Mesh.ff_iter(*crnt_f); ff_itr; ++ff_itr)
				{
					if (table[ff_itr.handle().idx()] != true)
					{
						new_found_faces.push_back(ff_itr.handle());
						table[ff_itr.handle().idx()] = true;
					}
				}
			}
			for (vector< FHandle >::iterator f_itr = last_found_face.begin(); f_itr != last_found_face.end(); ++f_itr)
			{
				candidate_faces.push_back(*f_itr);
			}
			last_found_face.assign(new_found_faces.begin(), new_found_faces.end());
		}
		delete table;
		/*----------------------------------------------------------------------*/
		/*對candidate faces做subdivision*/
		/*
			v0		vh		vg		v3
			*-------*-------*-------*
			|		|		|		|
			|		|		|		|
			|	  vi|	  vl|		|
		 va *-------*-------*-------*vf
			|		|		|		|
			|		|		|		|
			|	  vj|	  vk|		|
		 vb *-------*-------*-------*ve
			|		|		|		|
			|		|		|		|
			|		|		|		|
			*-------*-------*-------*
			v1		vc		vd		v2		
		*/
		for (vector< FHandle >::iterator face_itr = candidate_faces.begin(); face_itr != candidate_faces.end(); ++face_itr){
			VHandle v[4], va, vb, vc, vd, ve, vf, vg, vh, vi, vj, vk, vl;
			FVIter fv_itr = Mesh.fv_iter(*face_itr);
			for (int i = 0; fv_itr; ++fv_itr)
			{
				v[i++] = fv_itr.handle();
			}

			deleteFace(v[0], v[1], v[2], v[3]);
			va	 = addVertex((Mesh.point(v[0])*2+Mesh.point(v[1])  )/3);
			vb	 = addVertex((Mesh.point(v[0])  +Mesh.point(v[1])*2)/3);
			vc	 = addVertex((Mesh.point(v[1])*2+Mesh.point(v[2])  )/3);
			vd	 = addVertex((Mesh.point(v[1])  +Mesh.point(v[2])*2)/3);
			ve	 = addVertex((Mesh.point(v[2])*2+Mesh.point(v[3])  )/3);
			vf	 = addVertex((Mesh.point(v[2])  +Mesh.point(v[3])*2)/3);
			vg	 = addVertex((Mesh.point(v[3])*2+Mesh.point(v[0])  )/3);
			vh	 = addVertex((Mesh.point(v[3])  +Mesh.point(v[0])*2)/3);

			vi	 = addVertex((Mesh.point(vh)*2+Mesh.point(vc)  )/3);
			vj	 = addVertex((Mesh.point(vh)  +Mesh.point(vc)*2)/3);
			vk	 = addVertex((Mesh.point(vd)*2+Mesh.point(vg)  )/3);
			vl	 = addVertex((Mesh.point(vd)  +Mesh.point(vg)*2)/3);
			addFace(v[0], va, vi, vh);
			addFace(va, vb, vj, vi);
			addFace(vb, v[1], vc, vj);
			addFace(vc, vd, vk, vj);
			addFace(vd, v[2], ve, vk);
			addFace(ve, vf, vl, vk);
			addFace(vf, v[3], vg, vl);
			addFace(vg, vh, vi, vl);
			addFace(vi, vj, vk, vl);
		}
		/*----------------------------------------------------------------------*/
		deleteFace(vq, vw, vt, vr);//這行只是為了解決index問題
		/*----------------------------------------------------------------------*/
		return 0;
	}
	/*======================================================================*/
	int Model::findVertex(Point _p)
	{
		for( VIter v_itr = Mesh.vertices_begin(); v_itr != Mesh.vertices_end(); ++v_itr)
			if( Mesh.point(v_itr) == _p )
				return v_itr.handle().idx();
		return -1;
	}
	/*======================================================================*/
};
/*======================================================================*/

const OpenMesh::VertexHandle Tri_Mesh::InvalidVertexHandle;

void Tri_Mesh::Render_Solid(){
		FIter f_it;
		FVIter	fv_it;
		//glPushAttrib(GL_LIGHTING_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_LIGHTING);
		glPolygonOffset(2.0, 2.0);
		glBegin(GL_TRIANGLES);
		glColor4f(0.81, 0.74, 0.33, 0.3);
		for (f_it = faces_begin(); f_it != faces_end(); ++f_it) {
			for (fv_it = fv_iter( f_it ); fv_it; ++fv_it){						
				glNormal3dv(normal(fv_it.handle()).data());
				glVertex3dv(point(fv_it.handle()).data());
			}
		}
		glEnd();	
		
		glDisable(GL_POLYGON_OFFSET_FILL);
}

void Tri_Mesh::Render_SolidWireframe()
{
	FIter f_it;
	FVIter	fv_it;

	/*glPointSize(8.0);
	glBegin(GL_POINTS);
	glColor4f(1.0, 0.0, 0.0, 1.0);
	glVertex3dv(point(vertex_handle(0)));
	glEnd();*/
    glDisable(GL_LIGHTING);
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(2.0, 2.0);
	glBegin(GL_TRIANGLES);
	glColor4f(1.0, 0.96, 0.49, 1.0);
	for (f_it = faces_begin(); f_it != faces_end(); ++f_it) {
		for (fv_it = fv_iter( f_it ); fv_it; ++fv_it){						
			//glNormal3dv(normal(fv_it.handle()));
			glVertex3dv(point(fv_it.handle()).data());
		}
	}
	glEnd();
	
	//glDisable(GL_POLYGON_OFFSET_FILL);

	glPushAttrib(GL_LIGHTING_BIT);	
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	glColor3f(0.0, 0.0, 0.0);	
	glBegin(GL_LINES);
	for(OMT::EIter e_it = edges_begin(); e_it != edges_end(); ++e_it)
	{
		OMT::HEHandle _hedge = halfedge_handle(e_it.handle(),1);

		OMT::Point curVertex  = point(from_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());

		curVertex = point(to_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());			
	}
	glEnd();
	glPopAttrib();
}

void Tri_Mesh::loadToBuffer(Tri_Mesh _mesh, std::vector<double> & out_vertices, int & face, std::vector<double> & uv) {
	//to buffer , then shader
	FIter f_it;
	FVIter	fv_it;

	//face = 0;

	std::cout << "mesh support : " << std::endl;
	std::cout << "  " << "texcoords" << ": " << ((_mesh.has_vertex_texcoords1D()) ? "yes\n" : "no\n") << std::endl;
	std::cout << "  " << "texcoords" << ": " << ((_mesh.has_vertex_texcoords2D()) ? "yes\n" : "no\n") << std::endl;
	std::cout << "  " << "texcoords" << ": " << ((_mesh.has_vertex_texcoords3D()) ? "yes\n" : "no\n") << std::endl;
	std::cout << "  " << "normal" << ": " << ((_mesh.has_vertex_normals()) ? "yes\n" : "no\n") << std::endl;

	for (f_it = faces_begin(); f_it != faces_end(); ++f_it) {
		face++;
		for (fv_it = fv_iter(f_it); fv_it; ++fv_it) { // 每個面有三個點
			// 每個點有三個vertexes
			out_vertices.push_back(*(point(fv_it.handle()).data()));
			out_vertices.push_back(*(point(fv_it.handle()).data() + 1));
			out_vertices.push_back(*(point(fv_it.handle()).data() + 2));
			//std::cout << "x = " << *(point(fv_it.handle()).data()) << " y = " << *(point(fv_it.handle()).data() + 1) << " z = " << *(point(fv_it.handle()).data() + 2) << std::endl;
			uv.push_back(_mesh.texcoord2D(fv_it.handle())[0]);
			uv.push_back(_mesh.texcoord2D(fv_it.handle())[1]);

			//std::cout << "s = " << _mesh.texcoord2D(fv_it.handle())[0] << " t = " << _mesh.texcoord2D(fv_it.handle())[0] << std::endl;
			
		}
	}

}

void Tri_Mesh::loadToBufferPatch(std::vector<double> & out_vertices, int & face, std::vector<int> selectedFace, Tri_Mesh & patch) {
	FIter f_it;
	FVIter fv_it;
	VIter v_it;
	face = 0;
	std::vector<Tri_Mesh::VertexHandle> vhandle;
	std::vector<Tri_Mesh::VertexHandle>  face_vhandles;
	int verticesSeq[3] = { 0,0,0 };
	int verticesSeqIndex = 0;
	int isPatchHasPoint = 0;
	std::map<int, int> mapVerticesToVHandle;
	for (f_it = faces_begin(); f_it != faces_end(); ++f_it) {
		// check selected face
		int isFaceSelected = 0;
		for (int i = 0; i < selectedFace.size(); i++) {
			if (f_it.handle().idx() == selectedFace[i]) { // found face
				isFaceSelected = 1;
				break;
			}
			else if (f_it.handle().idx() > selectedFace[i] && i == selectedFace.size() - 1) isFaceSelected = 0; // face not selected
		}
		if (isFaceSelected == 0) continue;// goto next face if current not selected
		face++;
		for (fv_it = fv_iter(f_it); fv_it; ++fv_it) { // get verts of selected face
			out_vertices.push_back(*(point(fv_it.handle()).data()));
			out_vertices.push_back(*(point(fv_it.handle()).data() + 1));
			out_vertices.push_back(*(point(fv_it.handle()).data() + 2));
			//check repeat vert, if not => add to vhandle
			isPatchHasPoint = 0;
			int isSame = 0;
			for (v_it = patch.vertices_begin(); v_it != patch.vertices_end(); v_it++) {
				isPatchHasPoint = 1;
				//check repeat vert
				if ((out_vertices.at(out_vertices.size() - 3) == patch.point(v_it.handle())[0] && out_vertices.at(out_vertices.size() - 2) == patch.point(v_it.handle())[1] && out_vertices.at(out_vertices.size() - 1) == patch.point(v_it.handle())[2])) {
					verticesSeq[verticesSeqIndex++] = out_vertices.size() - 3;
					mapVerticesToVHandle[out_vertices.size() - 3] = v_it.handle().idx();
					isSame = 1;
					break;
				}
			}
			if (isSame) continue;
			if (isPatchHasPoint == 0) { // first vert
				//printf("first point...\n");
				vhandle.push_back(patch.add_vertex(Tri_Mesh::Point(out_vertices.at(0), out_vertices.at(1), out_vertices.at(2))));
				verticesSeq[verticesSeqIndex++] = out_vertices.size() - 3; // record out_vert position
				mapVerticesToVHandle[out_vertices.size() - 3] = patch.vertices_begin().handle().idx(); // mapping id between patch & out_vert
				//printf("mapVToH = %d : %d\n", out_vertices.size() - 3, patch.vertices_begin().handle().idx());
			}else{ // new point
				vhandle.push_back(patch.add_vertex(Tri_Mesh::Point(out_vertices.at(out_vertices.size() - 3), out_vertices.at(out_vertices.size() - 2), out_vertices.at(out_vertices.size() - 1))));
				verticesSeq[verticesSeqIndex++] = out_vertices.size() - 3;
				mapVerticesToVHandle[out_vertices.size() - 3] = (v_it).handle().idx();
				//printf("mapVToH = %d : %d\n", out_vertices.size() - 3, (v_it).handle().idx());
			}
		}
		//add face to patch from vhandle
		int num;
		face_vhandles.clear();
		for (int i = 0; i < 3; i++) {
			num = mapVerticesToVHandle.find(verticesSeq[i])->second;
			face_vhandles.push_back(vhandle[num]);
		}
		//std::cout << "add face to face_vhandles" << std::endl;
		patch.add_face(face_vhandles);
		verticesSeqIndex = 0;
	}
}

void Tri_Mesh::findNearestPoint(Tri_Mesh mesh, std::vector<double> mouse, int face, std::vector<double> &vertex) {
	FIter f_it;
	FVIter fv_it;

	VHandle min;

	int isFaceMatch = 0;
	for (f_it = faces_begin(); f_it != faces_end() && !isFaceMatch ; ++f_it) {

		//printf("out for\n");


		if (f_it.handle().idx() != face) continue;

		else min = fv_iter(f_it).handle();

		for (fv_it = ++fv_iter(f_it); fv_it; ++fv_it) { // 每個面有三個點
			//printf("in for\n");
			double temp = std::sqrtl(std::pow((point(min)[0] - mouse[0]), 2) + std::pow((point(min)[1] - mouse[1]), 2) + std::pow((point(min)[2] - mouse[2]), 2));
			double current = std::sqrtl(std::pow((point(fv_it.handle())[0] - mouse[0]), 2) + std::pow((point(fv_it.handle())[1] - mouse[1]), 2) + std::pow((point(fv_it.handle())[2] - mouse[2]), 2));

			printf(" temp %f  v.s. current %f\n", temp, current);
			std::cout << "cur iter point : " << point(fv_it.handle())[0] << "," << point(fv_it.handle())[1] << "," << point(fv_it.handle())[2] << std::endl;

			if (temp > current) min = fv_it.handle();


			isFaceMatch = 1;
		}


	}

	//printf("here?\n");

	for (int i = 0; i < 3 && isFaceMatch; i++) {
		vertex.push_back(point(min)[i]);
	}
	if (isFaceMatch) printf("selected point is : %f %f %f\n", vertex[0], vertex[1], vertex[2]);

}
//new func
void Tri_Mesh::findNearestVert(Tri_Mesh mesh, std::vector<double> mouse, int face, std::vector<double> &vertex, mat4 MVP,double dis) {
	FIter f_it;
	FVIter fv_it;
	VHandle min;
	int isFaceMatch = 0;
	for (f_it = faces_begin(); f_it != faces_end() && !isFaceMatch; ++f_it) {
		//find chosen face
		if (f_it.handle().idx() != face) continue;	
		else min = fv_iter(f_it).handle();
		// compare three vert
		for (fv_it = ++fv_iter(f_it); fv_it; ++fv_it) { 
			//put point into vec to mult MVP
			vec3 tempPoint , curPoint;
			for (int i = 0; i < 3; i++) {
				tempPoint[i] = point(min)[i];
				curPoint[i] = point(fv_it.handle())[i];
			}
			//fix range issue 
			vec4 tempPosOnScreen = MVP * vec4(tempPoint, 1) / pow(dis, 1.35) / 0.7;
			vec4 curPosOnScreen = MVP * vec4(curPoint,1) / pow(dis, 1.35) / 0.7;
			//compare mouse dis via iterator
			double temp = pow((tempPosOnScreen[0] - mouse[0]), 2) + pow((tempPosOnScreen[1] - mouse[1]), 2);
			double current = pow((curPosOnScreen[0] - mouse[0]), 2) + pow((curPosOnScreen[1] - mouse[1]), 2);
			if (temp > current) min = fv_it.handle();
			//cout << "cur iter point scr position: " << curPosOnScreen[0] << "," << curPosOnScreen[1] << std::endl;
			isFaceMatch = 1;
		}
	}
	for (int i = 0; i < 3 && isFaceMatch; i++) vertex.push_back(point(min)[i]);
	//if (isFaceMatch) printf("selected point is : %f %f %f\n", vertex[0], vertex[1], vertex[2]);
}

void Tri_Mesh::getUV(std::vector<double> & patchuv, Tri_Mesh patch, float uvRotateAngle) {
	VVIter vv_it;
	VIter v_it;
	EIter e_it;
	HHandle heh;
	FVIter fv_it;
	FIter f_it;

	//Step1 : 先找到第一個邊界
	for (e_it = edges_begin(); e_it != edges_end(); e_it++) {
		printf("enter edge iterator...\n");
		bool isBoundary = patch.is_boundary(*e_it);
		if (isBoundary) {
			heh = patch.halfedge_handle(e_it.handle(), 1);
			break;
		}
	}

	// Step2 : 沿著該邊界找尋下一個邊
	double perimeter = 0;
	std::vector<double> segLength;
	std::vector<Tri_Mesh::VertexHandle> vhs; // 儲存排序好的邊界點
	HHandle hehNext = heh;
	do {
		Point from = patch.point(patch.from_vertex_handle(hehNext));
		Point to = patch.point(patch.to_vertex_handle(hehNext));
		perimeter += (from - to).length(); // v0 - v? 的長度
		printf("perimeter = %f\n", perimeter);
		segLength.push_back(perimeter); // 存入vector中，以便之後做texcoord
		vhs.push_back(patch.from_vertex_handle(hehNext)); // 把邊界上的點一一存入
		hehNext = patch.next_halfedge_handle(hehNext); // 可以直接依序往下一個heh走
	} while (heh != hehNext);

	//Step3 : 找完所有的邊界點和距離後，可以將邊界已知點換成texcoord
	float rd = (225 + uvRotateAngle) * M_PI / 180.0;
	float initDist = 0;
	Tri_Mesh::TexCoord2D st(0, 0);
	float R = std::sqrt(2) / 2.0; // 根號2/2
	st[0] = R * cos(rd) + 0.5;
	st[1] = R * sin(rd) + 0.5;

	if (st[0] > 1){
		st[0] = 1;
		st[1] = tan(rd) / 2 + 0.5;
	}
	if (st[0] < 0){
		st[0] = 0;
		st[1] = 0.5 - tan(rd) / 2;
	}
	if (st[1] > 1){
		st[0] = tan(M_PI_2 - rd) / 2 + 0.5;
		st[1] = 1;
	}

	if (st[1] < 0){
		st[0] = 0.5 - tan(M_PI_2 - rd) / 2;
		st[1] = 0;
	}

	if (uvRotateAngle <= 90){
		initDist = st.length();
	}
	else if (uvRotateAngle <= 180){
		initDist = 1 + st[1];
	}
	else if (uvRotateAngle <= 270){
		initDist = 3 - st[0];
	}
	else{
		initDist = 4 - st[1];
	}

	patch.request_vertex_texcoords2D();
	patch.set_texcoord2D(vhs[0], st);
	perimeter /= 4.0;
	for (int i = 1; i < vhs.size(); ++i){
		double curLen = segLength[i - 1] / perimeter + initDist; // 意即L0n/Ltotal*4
		if (curLen > 4){
			curLen -= 4;
		}
		if (curLen <= 1){
			st[0] = curLen;
			st[1] = 0;
		}
		else if (curLen <= 2){
			st[0] = 1;
			st[1] = curLen - 1;}
		else if (curLen <= 3){
			st[0] = 3 - curLen;
			st[1] = 1;
		}else{
			st[0] = 0;
			st[1] = 4 - curLen;
		}
		patch.set_texcoord2D(vhs[i], st); // 先求出邊界點的uv座標，做為已知點
	}

	//Step4 : 接下來，算出非邊界的權重(使用均質公式)
	OpenMesh::HPropHandleT<double> heWeight;
	OpenMesh::VPropHandleT<int> row;
	patch.add_property(heWeight, "heWeight"); //加入property：權重
	patch.add_property(row, "row");

	for (e_it = patch.edges_begin(); e_it != patch.edges_end(); ++e_it) {
		bool isBound = patch.is_boundary(*e_it);
		if (!isBound) { // 如果不是邊界，就要算他的權重，兩個half_edge都要
			GLdouble angle1, angle2, w;
			Tri_Mesh::HalfedgeHandle _heh = patch.halfedge_handle(e_it.handle(), 0);
			Point pFrom = patch.point(patch.from_vertex_handle(_heh)); // 起點
			Point pTo = patch.point(patch.to_vertex_handle(_heh)); // 終點
			//問題：opposite_he_opposite_vh, opposite_vh抱錯
			Point p1 = patch.point(patch.opposite_vh(_heh)); // 線上對點
			Point p2 = patch.point(patch.opposite_he_opposite_vh(_heh)); // 線下對點
			//auto _vh = patch.opposite_vh(_heh);
			double edgeLen = (pFrom - pTo).length(); //線長度
			OpenMesh::Vec3d v1 = (OpenMesh::Vec3d)(pTo - pFrom);
			v1.normalize();
			OpenMesh::Vec3d v2 = (OpenMesh::Vec3d)(p1 - pFrom);
			v2.normalize();
			angle1 = std::acos(OpenMesh::dot(v1, v2)); //線上點與線的arccos，取得角度

			v2 = (OpenMesh::Vec3d)(p2 - pFrom);
			v2.normalize();

			angle2 = std::acos(OpenMesh::dot(v1, v2)); //線下點與線的arccos，取得角度
			//均質公式
			w = (std::tan(angle1 / 2.0f) + std::tan(angle2 / 2.0f)) / edgeLen;
			patch.property(heWeight, _heh) = w;  //將其中一個edge算好丟入邊的性質中

			// 再來計算反方向的halfedge weight
			v1 = -v1;
			v2 = (OpenMesh::Vec3d)(p1 - pTo);
			v2.normalize();
			angle1 = std::acos(OpenMesh::dot(v1, v2));//線上點與線的arccos，取得角度

			v2 = (OpenMesh::Vec3d)(p2 - pTo);
			v2.normalize();
			angle2 = std::acos(OpenMesh::dot(v1, v2));//線下點與線的arccos，取得角度

			w = (std::tan(angle1 / 2.0f) + std::tan(angle2 / 2.0f)) / edgeLen;
			patch.property(heWeight, patch.opposite_halfedge_handle(_heh)) = w;//將反方向edge算好丟入邊的性質中
		}
	}
	//Step5 : 找出矩陣的大小(NxN)，總點數-邊界點
	int count = 0;
	for (v_it = patch.vertices_begin(); v_it != patch.vertices_end(); v_it++) { // debug：patch.vertices_end()要加patch
		if (patch.is_boundary(*v_it)) patch.property(row, *v_it) = -1;
		else patch.property(row, *v_it) = count++;
	}
	//Step6 : 填寫矩陣
	typedef Eigen::SparseMatrix<double> SpMat;
	SpMat A(count, count);
	Eigen::VectorXd BX(count);
	Eigen::VectorXd BY(count);//x 和 y 各解一次
	Eigen::SimplicialLDLT<Eigen::SparseMatrix<double> > linearSolver;

	BX.setZero();
	BY.setZero();
	// fiil matrix
	for (v_it = patch.vertices_begin(); v_it != patch.vertices_end(); ++v_it){
		if (!patch.is_boundary(*v_it)){ // 未知點是我們要解的東西
			int i = patch.property(row, *v_it);
			double totalWeight = 0;

			for (vv_it = patch.vv_iter(*v_it); vv_it.is_valid(); ++vv_it){
				HHandle _heh = patch.find_halfedge(*v_it, *vv_it);
				double w = patch.property(heWeight, _heh);

				if (patch.is_boundary(*vv_it)){ //是邊界的為已知，填入B矩陣
					TexCoord2D texCoord = patch.texcoord2D(*vv_it);
					BX[i] += w * texCoord[0]; // 單行全相加
					BY[i] += w * texCoord[1];
				}
				else{ // 不是邊界的為未知點，填入A矩陣
					int j = patch.property(row, *vv_it);
					A.insert(i, j) = -w;
				}
				totalWeight += w;
			}
			A.insert(i, i) = totalWeight; // 這裡的解法不用讓所有的w除上大W
		}
	}
	A.makeCompressed();
	//Step7 : 解開未知的uv座標們
	SpMat At = A.transpose();
	linearSolver.compute(At * A);

	Eigen::VectorXd TX = linearSolver.solve(At * BX);
	Eigen::VectorXd TY = linearSolver.solve(At * BY);
	for (v_it = patch.vertices_begin(); v_it != patch.vertices_end(); ++v_it){
		if (!patch.is_boundary(*v_it)){
			int i = patch.property(row, *v_it);
			patch.set_texcoord2D(*v_it, TexCoord2D(TX[i], TY[i]));
		}
	}
	for (f_it = faces_begin(); f_it != faces_end(); ++f_it) {
		for (fv_it = fv_iter(f_it); fv_it; ++fv_it)
		{
			patchuv.push_back(patch.texcoord2D(*fv_it)[0]);
			patchuv.push_back(patch.texcoord2D(*fv_it)[1]);
			//patchuv.push_back(*(point(fv_it.handle()).data()));
			//patchuv.push_back(*(point(fv_it.handle()).data() + 1));
		}
	}
	//printf("patchuv : \n");
	//printf("patchuv.size() : %d\n", patchuv.size());
	//for (int i = 0; i < patchuv.size(); i += 2) printf("s = %f , t = %f\n", patchuv.at(i), patchuv.at(i + 1));
}

void Tri_Mesh::Render_Wireframe(){
	//glPushAttrib(GL_LIGHTING_BIT);	
	glDisable(GL_LIGHTING);
	glLineWidth(1.0);
	
	glColor3f(0.0, 0.0, 0.0);	

	glBegin(GL_LINES);
	for(OMT::EIter e_it = edges_begin(); e_it != edges_end(); ++e_it)
	{
		OMT::HEHandle _hedge = halfedge_handle(e_it.handle(),1);

		OMT::Point curVertex  = point(from_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());

		curVertex = point(to_vertex_handle(_hedge));
		glVertex3dv(curVertex.data());			
	}
	glEnd();
	
}

void Tri_Mesh::Render_Point()
{
	glPointSize ( 8.0 ) ;				  
	glColor3f( 1.0, 0.0, 0.0 ) ;
	glBegin(GL_POINTS);
	for (OMT::VIter v_it = vertices_begin() ; v_it != vertices_end() ; ++v_it)
	{
		  glVertex3dv(point(v_it).data());
	}
	glEnd();
}

bool ReadFile(std::string _fileName,Tri_Mesh *_mesh)
{
	bool isRead = false;
	OpenMesh::IO::Options opt;
	opt += OpenMesh::IO::Options::VertexTexCoord;
	if ( OpenMesh::IO::read_mesh(*_mesh, _fileName, opt) )
	{
		//read mesh from filename OK!
		isRead = true;
	}
	if(isRead)
	{
		// If the file did not provide vertex normals and mesh has vertex normal ,then calculate them
		if (!opt.check( OpenMesh::IO::Options::VertexNormal ) && _mesh->has_vertex_normals())
		{
			_mesh->update_normals();
		}
	}

	printf("opt.check( OpenMesh::IO::Options::VertexNormal ) = %d\n", opt.check(OpenMesh::IO::Options::VertexNormal));
	printf("_mesh->has_vertex_normals() = %d\n", _mesh->has_vertex_normals());

	printf("opt.check( OpenMesh::IO::Options::VertexTexCoord ) = %d\n", opt.check(OpenMesh::IO::Options::VertexTexCoord));
	printf("_mesh->has_vertex_texcoord2D() = %d\n", _mesh->has_vertex_texcoords2D());

	return isRead;
}

bool SaveFile(std::string _fileName, Tri_Mesh *_mesh){
	bool isSave = false;
	OpenMesh::IO::Options opt = OpenMesh::IO::Options::VertexTexCoord;
	opt += OpenMesh::IO::Options::VertexNormal;
	if (OpenMesh::IO::write_mesh(*_mesh, _fileName, opt))isSave = true;
	return isSave;
}
