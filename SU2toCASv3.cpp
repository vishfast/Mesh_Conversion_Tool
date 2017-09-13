#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

struct f3{

	int node3;
	int cell_l;
	int cell_r;
	f3* next;

};

struct f2{

	int node2;
	int size;
	f3* ptr;
	f2* next;

};

struct f1{

	int size;
	f2* ptr;

};

void sort4 (int* array){

    int dummy;
    if(array[0]>array[1]){

        dummy = array[0];
        array[0] = array[1];
        array[1] = dummy;

    }
    if(array[2]>array[3]){

        dummy = array[2];
        array[2] = array[3];
        array[3] = dummy;

    }
    if(array[1]>array[2]){

        dummy = array[1];
        array[1] = array[2];
        array[2] = dummy;

        if(array[0]>array[1]){

            dummy = array[0];
            array[0] = array[1];
            array[1] = dummy;

        }
        if(array[2]>array[3]){

            dummy = array[2];
            array[2] = array[3];
            array[3] = dummy;

        }

    }

}

int check_lr(int nd1, int nd2, int nd3, int nd4, double** point){

    double xm, ym, zm;
    double x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
    
    x1 = point[nd1][0];
    y1 = point[nd1][1];
    z1 = point[nd1][2];

    x2 = point[nd2][0];
    y2 = point[nd2][1];
    z2 = point[nd2][2];

    x3 = point[nd3][0];
    y3 = point[nd3][1];
    z3 = point[nd3][2];

    x4 = point[nd4][0];
    y4 = point[nd4][1];
    z4 = point[nd4][2];

    double result = (x4-x1)*((y2-y1)*(z3-z2)-(z2-z1)*(y3-y2)) + (y4-y1)*((z2-z1)*(x3-x2)-(x2-x1)*(z3-z2)) + (z4-z1)*((x2-x1)*(y3-y2)-(y2-y1)*(x3-x2));

    if(result>0.0) return 0; else return 1;

}

void place(int cellid, int nd1, int nd2, int nd3, int nd4, double** points, f1* faces, ofstream& outfile){

	int chk;

	if (nd4>=0) chk = check_lr(nd1, nd2, nd3, nd4, points); else chk = -1;

	f2* ptr2 = faces[nd1].ptr;

	for(int i=0; i<faces[nd1].size; i++){

		if ((ptr2->node2)==nd2) break; else ptr2 = ptr2->next;

	}

	if (ptr2==NULL){
	
		f2* L2 = new f2;
		L2->node2 = nd2;
		L2->ptr   = NULL;
		L2->next  = faces[nd1].ptr;
		L2->size  = 0;
		faces[nd1].size++;
		ptr2     = L2;
		faces[nd1].ptr = L2;

	}

	f3* ptr3 = ptr2->ptr;

	for(int i=0; i<(ptr2->size); i++){

		if ((ptr3->node3)==nd3) break; else ptr3 = ptr3->next;

	}

	if (ptr3==NULL){
	
		f3* L3 = new f3;
		L3->node3 = nd3;
		L3->next  = ptr2->ptr;
		L3->cell_l = -1;
		L3->cell_r = -1;
		ptr2->size++;
		ptr3      = L3;
		ptr2->ptr = L3;

	}
	
	if (chk==0) ptr3->cell_r=cellid; else if(chk==1) ptr3->cell_l=cellid; else { if((ptr3->cell_r)==-1) ptr3->cell_r = 0; else ptr3->cell_l = 0; }

	if ((ptr3->cell_l)>=0 && (ptr3->cell_r)>=0) {outfile<<hex<<nd1+1<<"\t"<<nd2+1<<"\t"<<nd3+1<<"\t"<<(ptr3->cell_r)<<"\t"<<(ptr3->cell_l)<<'\r'<<endl;}

}

int main(int argc, char **argv){

	if(argc!=3){ cout<<"Invalid syntax: Correct syntax is \"<executable> <su2_file_name> <cas_output_name>\""<<endl; }

    else{

    ifstream infile (argv[1]);	// CHANGE THIS TO THE NAME OF THE INPUT GRID FILE (ENSURE THAT ALL COMMENTS FROM THE FILE HAVE BEEN REMOVED)
													// SEQUENCE : ELEMENTS, POINTS, BOUNDARIES (CURRENTLY FOR TETRAHEDRAL 3D GRIDS ONLY)

    string s;										// DUMMY STRING TO TAKE CARE OF THE TEXT IN THE MESH FILE
    int elemtype=0;									// CORRESPONDS TO THE SHAPE OF THE ELEMENT (CELL OR FACE IN CONSIDERATION)
    int dim=0;										// DIMENSION OF THE GRID
    int nelem=0;									// NUMBER OF CELLS IN THE VOLUME
    int npoint=0;									// NUMBER OF NODES IN THE GRID
    int nsurf=0;									// NUMBER OF DIFFERENT BOUNDARY FACE GROUPS (GROUPED ACCORDING TO SIMILAR BOUNDARY CONDITION OR GEOMETRY)
    int elemid=0;									// ID OF THE CELL BEING RETRIEVED
    int pointid=0;									// ID OF THE POINT BEING RETRIEVED

    ////////////////////////////////////////////////////////////////////////////////////

    infile>>s>>dim;
    infile>>s>>nelem;

    ////////////////////////////////////////////////////////////////////////////////////

    int** elements = new int* [nelem];				// 2D ARRAY OF SU2 CELL CONNECTIVITY

    // EXTRACTING CELL CONNECTIVITY

    for(int i=0; i<nelem; i++){
 
        elements[i] = new int [4];
        infile>>elemtype>>elements[i][0]>>elements[i][1]>>elements[i][2]>>elements[i][3]>>elemid;
        sort4(elements[i]);

    }

    ////////////////////////////////////////////////////////////////////////////////////

    infile>>s>>npoint;

    double** points = new double* [npoint];			// 2D ARRAY OF POINT COORDINATES

    for(int i=0; i<npoint; i++){
 
        points[i] = new double [3];
        infile>>points[i][0]>>points[i][1]>>points[i][2]>>pointid;
 
    }

    ////////////////////////////////////////////////////////////////////////////////////

    infile>>s>>nsurf;
    int* nelemloc = new int [nsurf];				// ARRAY OF NUMBER OF FACES ON EACH BOUNDARY BLOCK
    int nbface = 0;
    int dummy;
    int*** localelements = new int** [nsurf];		// 3D ARRAY OF SU2 BOUNDARY FACE CONNECTIVITY
    for(int i=0; i<nsurf; i++){
 
        infile>>s>>dummy;
        infile>>s>>nelemloc[i];
        nbface+=nelemloc[i];
        localelements[i] = new int* [nelemloc[i]];
        for(int j=0; j<nelemloc[i]; j++){
 
            localelements[i][j] = new int [3];
            infile>>elemtype>>localelements[i][j][0]>>localelements[i][j][1]>>localelements[i][j][2];
 
        }
 
    }
 
    infile.close();

    ////////////////////////////////////////////////////////////////////////////////////

    f1* faces = new f1 [npoint];

    ofstream outfile (argv[2]);					// CHANGE THIS TO THE DESIRED NAME OF THE OUTPUT GRID FILE

	outfile<<"(0 \"Dimensions:\")\r"<<endl;
    outfile<<"(2 3)\r"<<endl;
    outfile<<"\r"<<endl;
    outfile<<"(0 \"Grid:\")\r"<<endl;
    outfile<<"\r"<<endl;
    outfile<<"(12 (0 1 "<<hex<<nelem<<" 0))\r"<<endl;
    outfile<<"(13 (0 1 "<<hex<<(int)(nelem*2+0.5*nbface)<<" 0))\r"<<endl;
    outfile<<"(10 (0 1 "<<hex<<npoint<<" 0 3))\r"<<endl;
    outfile<<"\r"<<endl;
    
    for(int i=0; i<npoint; i++){

    	faces[i].size = 0;
    	faces[i].ptr  = NULL;

    }

    outfile<<"(12 (2 1 "<<hex<<nelem<<" 1 2))\r"<<endl;
    outfile<<"\r"<<endl;

    int counter=(int)(nelem*2-0.5*nbface);

    outfile<<"(13 (3 1 "<<hex<<counter<<" 2 3)(\r"<<endl;
	for(int i=0; i<nelem; i++){

        place(i+1, elements[i][0], elements[i][1], elements[i][2], elements[i][3], points, faces, outfile);
        place(i+1, elements[i][0], elements[i][1], elements[i][3], elements[i][2], points, faces, outfile);
        place(i+1, elements[i][0], elements[i][2], elements[i][3], elements[i][1], points, faces, outfile);
        place(i+1, elements[i][1], elements[i][2], elements[i][3], elements[i][0], points, faces, outfile);

        /////////////////////////////////////////////////////////////////////
        cout<<i+1<<"/"<<nelem<<" cells mapped"<<'\r';
        cout.flush();
    
    }

    outfile<<"))\r"<<endl;
    outfile<<"\r"<<endl;

    cout<<endl;

    for(int i=0; i<nsurf; i++){

    	outfile<<"(0 \"Physical Boundary "<<i+1<<"\")\r"<<endl;

    	outfile<<"(13 ("<<hex<<i+4<<" "<<counter+1<<" "<<counter+nelemloc[i]<<" 3 3)(\r"<<endl;

        counter = counter+nelemloc[i];
    	
        for(int j=0; j<nelemloc[i]; j++){

            int bnd[4];
            bnd[0] = -1;
            bnd[1] = localelements[i][j][0];
            bnd[2] = localelements[i][j][1];
            bnd[3] = localelements[i][j][2];
            sort4(bnd);

	        place(0, bnd[1], bnd[2], bnd[3], -1, points, faces, outfile);
    	    
    	    //////////////////////////////////////////////////////////////////////
    	    cout<<j+1<<"/"<<nelemloc[i]<<" surfaces of Boundary "<<i+1<<" mapped"<<'\r';
        	cout.flush();

    	}

    	outfile<<"))\r"<<endl;
    	outfile<<"\r"<<endl;

    	cout<<endl;

    }

    outfile<<"(10 (1 1 "<<hex<<npoint<<" 1 3)(\r"<<endl;

    for(int i=0; i<npoint; i++){

        outfile<<points[i][0]<<"\t\t"<<points[i][1]<<"\t\t"<<points[i][2]<<'\r'<<endl;

    }

    outfile<<"))\r"<<endl;
    outfile<<"\r"<<endl;

    outfile.close();

    }

}