
/*
DTW prototype version 0.1 by PedroLopes (plopes)
-------------------------
This external object was written to puredata. It computes the similarity value between to series of numbers, received on the inlets as a pd-list, using the Dynamic Time Warping algorithm (DTW).
-------------------------
The DTW implementation is based on Andrew Slater and John Coleman's DTW code written in C, and available here: 
- http://www.phon.ox.ac.uk/files/slp/Extras/dtw.html
-------------------------
The array size inlets are based on the arraysize code available on puredata/SVN which in its turn is based on arraysize from pixlib, this here:
- http://pix.test.at/pd/pixlib
*/


#include <m_pd.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



#define VERY_BIG  (1e30)

//ENUM template_num {ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGTH}




clock_t c1;
clock_t c2;

time_t start,end;


//---------------------------------------

static t_class *dtw_class;

typedef struct _dtw {
  t_object  x_obj;
  t_symbol *array_name;
  int size;
} t_dtw;



// our struct

typedef struct gesture {
  char templatename[9];
  char* gesturename;
  float threshold;
} t_gesture;



 void dtw_set(t_dtw *x, t_symbol *s, int array_num)
 {
//   t_garray *garray;
//   printf("nha0");
// 
//   x->array_name = s;
// 
// if (array_num == 1)
//   input = s->s_name;
// else 
//   template = s->s_name;
// 
// 
//   post("strname:%s", input);
//   post("strname:%s", template);
//   printf("nha1");
// 
//   if(!(garray = (t_garray *)pd_findbyclass(x->array_name,garray_class))) {
//     pd_error(x, "%s: no such table", x->array_name->s_name);
//   } else {
//     //outlet_float(x->x_obj.ob_outlet, garray_npoints(garray));
//     post("SIZE: %d", garray_npoints(garray));
//   }
// 
 }

void setarray1(t_dtw *x, t_symbol *s) 
{
  post("calling1");
  dtw_set(x, s, 1);
}
void setarray2(t_dtw *x, t_symbol *s) 
{
  post("calling2");
  dtw_set(x, s, 2);
}

void dtw_size(t_dtw *x, t_float *s)
{
  float var = atof("0.1");;
  //not working
  //x->size = atoi(*s);
  post("Set size not working in this version:%f",var);
}


void dtw_bang(t_dtw *pdx)
{

double dif;

c1=clock();
time(&start);

  //t_garray *garray;
double **globdist;
double **Dist;

double top, mid, bot, cheapest, total;
unsigned short int **move;
unsigned short int **warp;
unsigned short int **temp;

float **x, **y;

unsigned int I, X, Y, n, i, j, k;
//array size is not working in this version (should be fixed soon)
unsigned int xsize;
unsigned int ysize;
//params is bound to 1 for now
unsigned int params = 1;
unsigned int debug; /* debug flag */

//new stuff
FILE *file1, *file2, *glob, *debug_file, *output_file;
char* input = "input";
char template[10] = {'t', 'e','m','p','l','a','t','e','3','\0'}; 
int num_templates = 8;
char c;
int f = 1;
t_gesture* gestures[num_templates];

float diffs[num_templates];

int p;
for (p = 0; p < num_templates; p++)
  diffs[p]=VERY_BIG;

post("nha");
for (p = 0; p < num_templates; p++)
  post("%f",diffs[p]);
post("nha");

 /* open INPUT-parameter file */

if ((file1=fopen(input,"rb"))==NULL)
{fprintf(stderr,"File %s cannot be opened\n",input);
exit(1);
}

{//-------HARDCODED TEMPLATES

t_gesture *temp1 = (t_gesture*) malloc(sizeof(t_gesture));
t_gesture *temp2 = (t_gesture*) malloc(sizeof(t_gesture));
t_gesture *temp3 = (t_gesture*) malloc(sizeof(t_gesture));
t_gesture *temp4 = (t_gesture*) malloc(sizeof(t_gesture));
t_gesture *temp5 = (t_gesture*) malloc(sizeof(t_gesture));
t_gesture *temp6 = (t_gesture*) malloc(sizeof(t_gesture));
t_gesture *temp7 = (t_gesture*) malloc(sizeof(t_gesture));
t_gesture *temp8 = (t_gesture*) malloc(sizeof(t_gesture));
 	
  memcpy(temp1->templatename, "template1", strlen("template1")+1);
  temp1->gesturename = "kick1";
  temp1->threshold = 1.5;
  
  gestures[0]=temp1;
  
  memcpy(temp2->templatename, "template2", strlen("template2")+1);
  temp2->gesturename = "kick2";
  temp2->threshold = 0.8;
  
  gestures[1]=temp2;
  
  memcpy(temp3->templatename, "template3", strlen("template3")+1);
  temp3->gesturename = "kick3";
  temp3->threshold = 0.7;
  
  gestures[2]=temp3;
  
  memcpy(temp4->templatename, "template4", strlen("template4")+1);
  temp4->gesturename = "kick4";
  temp4->threshold = 1.7;
  
  gestures[3]=temp4;
  
  memcpy(temp5->templatename, "template5", strlen("template5")+1);
  temp5->gesturename = "drag1";
  temp5->threshold = 5.0;
  
  gestures[4]=temp5;
  
  memcpy(temp6->templatename, "template6", strlen("template6")+1);
  temp6->gesturename = "drag2";
  temp6->threshold = 5.0;
  
  gestures[5]=temp6;
  
  memcpy(temp7->templatename, "template7", strlen("template7")+1);
  temp7->gesturename = "drag3";
  temp7->threshold = 5.0;
  
  gestures[6]=temp7;
  
  memcpy(temp8->templatename, "template8", strlen("template8")+1);
  temp8->gesturename = "drag4";
  temp8->threshold = 5.0;
  
  gestures[7]=temp8;
  
}//-------HARDCODED TEMPLATES  

//dynamic templates
/*while (f <= num_templates)
{
  t_gesture *temp = (t_gesture*) malloc(sizeof(t_gesture));
  
  //build name
	c = f + '0';
	template[8] = c;

  memcpy(temp->templatename,template,strlen(template)+1);
  temp->gesturename = "kick";
  temp->threshold = 0.7;
  
  gestures[f-1]=temp;
  
  f++;
}
f=1;*/

while (f <= num_templates)
{
  
  fprintf(stderr,"\n%s\n", gestures[f-1]->templatename);
  
  f++;
}
f=1;

while (f <= num_templates)
{
  t_gesture *target = gestures[f-1];
  
  xsize = 100;
  ysize = 100;
    
  
  fseek(file1,0,SEEK_SET);

 /* apgar
	fprintf(stderr, "nha:%s\n", template); 
	c = f + '0';
	fprintf(stderr, "teste:%c\n", c); 
	template[8] = c;
	fprintf(stderr, "obj:%s\n", template); 
	/* open TEMPLATE-parameter file */

	
post("START---------------%s--------------",target->templatename);
	
if ((file2=fopen(target->templatename,"rb"))==NULL)
{fprintf(stderr,"File %s cannot be opened\n",target->templatename);
//exit(1);
}

fseek(file2,0,SEEK_SET);

//debug prints
//printf("xsize:%d,ysize:%d,params:%d\n",xsize,ysize,params);

     if ((debug_file = fopen("debugDTW.data","wb")) == NULL)
       {fprintf(stderr,"Cannot open debug file\n");
       exit(1);
       }

//Debug constant setup
     debug = 0;
  

  
     
if (debug==1) fprintf(debug_file,"xsize %d ysize %d params %d\n",xsize,ysize,params);

/* allocate memory for x and y matrices */

c1 = clock();

if ((x = malloc(xsize * sizeof(float *))) == NULL)
     fprintf(stderr,"Memory allocation error (x)\n");

for (i=0; i < xsize; i++)
     if ((x[i] = malloc(params * sizeof(float))) == NULL)
     fprintf(stderr,"Memory allocation error (x)\n");
  
if ((y = malloc(ysize * sizeof(float *))) == NULL)
     fprintf(stderr,"Memory allocation error (y)\n");

for (i=0; i < ysize; i++)
     if ((y[i] = malloc(params * sizeof(float))) == NULL)
     fprintf(stderr,"Memory allocation error (y)\n");

     /* allocate memory for Dist */

if ((Dist = malloc(xsize * sizeof(double *))) == NULL)
     fprintf(stderr,"Memory allocation error (Dist)\n");

for (i=0; i < xsize; i++)
if ((Dist[i] = malloc(ysize * sizeof(double))) == NULL)
     fprintf(stderr,"Memory allocation error (Dist)\n");

     /* allocate memory for globdist */

if ((globdist = malloc(xsize * sizeof(double *))) == NULL)
     fprintf(stderr,"Memory allocation error (globdist)\n");

for (i=0; i < xsize; i++)
if ((globdist[i] = malloc(ysize * sizeof(double))) == NULL)
     fprintf(stderr,"Memory allocation error (globdist)\n");

     /* allocate memory for move */

if ((move = malloc(xsize * sizeof(short *))) == NULL)
     fprintf(stderr,"Memory allocation error (move)\n");

for (i=0; i < xsize; i++)
if ((move[i] = malloc(ysize * sizeof(short))) == NULL)
     fprintf(stderr,"Memory allocation error (move)\n");

     /* allocate memory for temp */

if ((temp = malloc(xsize * 2 * sizeof(short *))) == NULL)
     fprintf(stderr,"Memory allocation error (temp)\n");

for (i=0; i < xsize*2; i++)
if ((temp[i] = malloc(2 * sizeof(short))) == NULL)
     fprintf(stderr,"Memory allocation error (temp)\n");

     /* allocate memory for warp */

if ((warp = malloc(xsize * 2 * sizeof(short *))) == NULL)
     fprintf(stderr,"Memory allocation error (warp)\n");

for (i=0; i < xsize*2; i++)
if ((warp[i] = malloc(2 * sizeof(short))) == NULL)
     fprintf(stderr,"Memory allocation error (warp)\n");


//read input
for (i=0; i < xsize; i++)
{
  for (k=0; k < params; k++)
    {
if (feof(file1))
  {fprintf(stderr,"Premature EOF in %s\n",input);
  exit(1);
  }

  int retf = fscanf(file1,"%f ",&x[i][k]);
  //post("value1-m:%f",x[i][k]);
  if (retf == -1)
  	post("scan error");
  

if (debug == 1)
  fprintf(debug_file,"float_x[%d %d] = %f\n",i,k,x[i][k]);
    }
}

/*read y=template parameter in y[]*/

for (i=0; i < ysize; i++)
{
  for (k=0; k < params; k++)
    {

if (feof(file2))
  {fprintf(stderr,"Premature EOF in %s\n",template);
  exit(1);
  }

int retf= fscanf(file2,"%f ",&y[i][k]);
if (retf == -1) post("scan error");
//post("value2:%f",x[i][k]);

 if (debug == 1)
   fprintf(debug_file,"float_y[%d %d] = %f\n",i,k,y[i][k]);
    }
}

//debug prints
//post("Computing distance matrix ...\n");

//current

/*Compute distance matrix*/

for(i=0;i<xsize;i++) {
  for(j=0;j<ysize;j++) {
    total = 0;
    for (k=0;k<params;k++) {
      total = total + ((x[i][k] - y[j][k]) * (x[i][k] - y[j][k]));
    }

    Dist[i][j] = total;

    if (debug == 1)
      fprintf(debug_file,"Dist: %d %d %.0f %.0f\n",i,j,total,Dist[i][j]);
  }
}

//debug prints
//post("Warping in progress ...\n");

/*% for first frame, only possible match is at (0,0)*/

globdist[0][0] = Dist[0][0];
for (j=1; j<xsize; j++)
	globdist[j][0] = VERY_BIG;

globdist[0][1] = VERY_BIG;
globdist[1][1] = globdist[0][0] + Dist[1][1];
move[1][1] = 2;

for(j=2;j<xsize;j++)
	globdist[j][1] = VERY_BIG;

for(i=2;i<ysize;i++) {
	globdist[0][i] = VERY_BIG;
	globdist[1][i] = globdist[0][i-1] + Dist[1][i];
	if (debug == 1)
	  fprintf(debug_file,"globdist[2][%d] = %.2e\n",i,globdist[2][i]);

	for(j=2;j<xsize;j++) {
		top = globdist[j-1][i-2] + Dist[j][i-1] + Dist[j][i];
		mid = globdist[j-1][i-1] + Dist[j][i];
		bot = globdist[j-2][i-1] + Dist[j-1][i] + Dist[j][i];
		if( (top < mid) && (top < bot))
		{
		cheapest = top;
		I = 1;
		}
	else if (mid < bot)
		{
		cheapest = mid;
		I = 2;
		}
	else {cheapest = bot;
		I = 3;
		}

/*if all costs are equal, pick middle path*/
       if( ( top == mid) && (mid == bot))
	 I = 2;

	globdist[j][i] = cheapest;
	move[j][i] = I;
	if (debug == 1) {
	  fprintf(debug_file,"globdist[%d][%d] = %.2e\n",j,i,globdist[j][i]);
	  fprintf(debug_file,"move j:%d:i:%d=%d\n",j,i,move[j][i]);
	      }
      }
}

if (debug == 1) {
  for (j=0; j<xsize; j++) {
    for (i=0; i<ysize; i++) {
      fprintf(debug_file,"[%d %d] globdist: %.2e    move: %d    \n",j,i,globdist[j][i],move[j][i]);
    }
  }
}



X = ysize-1; Y = xsize-1; n=0;
warp[n][0] = X; warp[n][1] = Y;


while (X > 0 && Y > 0) {
n=n+1;


if (n>ysize *2) {fprintf (stderr,"Warning: warp matrix too large!");
exit(1);
} 

 if (debug == 1)
   fprintf(debug_file,"Move %d %d %d\n", Y, X, move[Y][X]);

if (move[Y] [X] == 1 )
	{
	warp[n][0] = X-1; warp[n][1] = Y;
	n=n+1;
	X=X-2; Y = Y-1;
	}
else if (move[Y] [X] == 2)
	{
	X=X-1; Y = Y-1;
	}
else if (move[Y] [X] == 3 )
	{
	warp[n] [0] = X;
	warp[n] [1] = Y-1; 
	n=n+1;
	X=X-1; Y = Y-2;
      }
else {fprintf(stderr,"Error: move not defined for X = %d Y = %d\n",X,Y); 
}
warp[n] [0] =X;
warp[n] [1] =Y;

}


/*flip warp*/
for (i=0;i<=n;i++) {
  temp[i][0] = warp[n-i][0];
  temp[i][1] = warp[n-i][1];

}

for (i=0;i<=n;i++) {
  warp[i][0] = temp[i][0];
  warp[i][1] = temp[i][1];

}

//debug prints
//post("Warping complete. Writing output file.\n");

//debug prints
//post("%f\n",globdist[xsize-1][ysize-1]);
//fprintf(stdout,"%s     %s     %.3f\n",input,template,globdist[xsize-1][ysize-1]);

outlet_float(pdx->x_obj.ob_outlet, globdist[xsize-1][ysize-1]);


//target->templatename
if (globdist[xsize-1][ysize-1] <= target->threshold)
{
  //Identified gesture under threshold
  post("Found gesture: %s, in %s with value %f under threshold %f", target->gesturename, target->templatename, globdist[xsize-1][ysize-1], target->threshold);
  //fprintf(stderr,"Found gesture: %s, in %s with value %f under threshold %f\n", target->gesturename, target->templatename, globdist[xsize-1][ysize-1], target->threshold);
  
  diffs[f-1]=fabsf(globdist[xsize-1][ysize-1] - target->threshold);
  post("diffs:%f",diffs[f-1]);
}  
else 
{
 post("Did not match current gesture, reiterate."); 
 //fprintf(stderr,"Did not match current gesture, reiterate.\n"); 
} 

f++;
post("END---------------%s--------------",target->templatename);
}

float min=VERY_BIG;
t_gesture* t_min;
//Let's analyze the data!
for (p = 0; p < num_templates; p++)
{
  post("%f",diffs[p]);
  
  if (diffs[p] < min)
  {
    t_min = gestures[p];
    min = diffs[p];
  }
}

c2 = clock();
post("DTW for current input and template DB is timecpu:%d", (c2-c1)/CLOCKS_PER_SEC);

if (min == VERY_BIG)
{
  post("======================");
  post("Gesture not found in DB");
}
else
{
 post("======================");
  post("Best gesture is %s", t_min->gesturename);
}


//end of func
} 

/*void arraysize_bang(t_arraysize *x)
{
  t_garray *garray;

  if(!(garray = (t_garray *)pd_findbyclass(x->array_name,garray_class))) {
    pd_error(x, "%s: no such table", x->array_name->s_name);
  } else {
    //outlet_float(x->x_obj.ob_outlet, garray_npoints(garray));
    post("SIZE: %d", garray_npoints(garray));
  }
}*/



void my_list_method(t_dtw *x, t_symbol *s, int argc, t_atom *argv)
{
  int i = 0;
  t_float f1=0, f2=0;

  while(i<argc)
  {	
	f1=atom_getfloat(argv+i);
	post("atom:%f",atom_getfloat(argv+i));
	i++;
  }

} 

void *dtw_new(t_symbol *s)
{
  t_dtw *x = (t_dtw *)pd_new(dtw_class);

  symbolinlet_new(&x->x_obj, &x->array_name);
  outlet_new(&x->x_obj, gensym("float"));

  x->array_name = s;

  return (void *)x;
}

void dtw_setup(void)
{
  dtw_class = class_new(gensym("dtw"), (t_newmethod)dtw_new, 0, sizeof(t_dtw), CLASS_DEFAULT, A_DEFSYMBOL, 0);
  
  class_addmethod(dtw_class,(t_method)setarray1,gensym("set1"), A_DEFSYMBOL, 0);
  class_addmethod(dtw_class,(t_method)setarray2,gensym("set2"), A_DEFSYMBOL, 0);
  class_addbang(dtw_class,dtw_bang);
  class_addlist(dtw_class, my_list_method);
  class_addmethod(dtw_class,(t_method)dtw_size,gensym("size"), A_FLOAT, 0);

}
