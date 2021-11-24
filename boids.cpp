
    #include <cstdlib>
    #include <iostream>
    #include <cmath>
    #include <vector>
    #include <fstream>
    #include <GL/glew.h>
    #include <GL/freeglut.h>
  //  #include "gltools.h"
    using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct imageFile
{
	int width;
	int height;
	unsigned char *data;
};

imageFile *getBMP(std::string fileName)
{
	int offset, // No. of bytes to start of image data in input BMP file.
		w, // Width in pixels of input BMP file.
		h; // Height in pixels of input BMP file.

	// Initialize imageFile objects.
	imageFile *tempStore = new imageFile; // Temporary storage.
	imageFile *outRGB = new imageFile; // RGB output file.
	imageFile *outRGBA = new imageFile; // RGBA output file.

	// Initialize input stream.
	std::ifstream inFile(fileName.c_str(), std::ios::binary);

	// Get start point of image data in input BMP file.
	inFile.seekg(10);
	inFile.read((char *)&offset, 4);

	// Get image width and height.
	inFile.seekg(18);
	inFile.read((char *)&w, 4);
	inFile.read((char *)&h, 4);

	// Determine the length of padding of the pixel rows
	// (each pixel row of a BMP file is 4-byte aligned by padding with zero bytes).
	int padding = (3 * w) % 4 ? 4 - (3 * w) % 4 : 0;

	// Allocate storage for temporary input file, read in image data from the BMP file, close input stream.
	tempStore->data = new unsigned char[(3 * w + padding) * h];
	inFile.seekg(offset);
	inFile.read((char *)tempStore->data, (3 * w + padding) * h);
	inFile.close();

	// Set image width and height and allocate storage for image in output RGB file.
	outRGB->width = w;
	outRGB->height = h;
	outRGB->data = new unsigned char[3 * w * h];

	// Copy data from temporary input file to output RGB file adjusting for padding and performing BGR to RGB conversion.
	int tempStorePos = 0;
	int outRGBpos = 0;
	for (int j = 0; j < h; j++)
		for (int i = 0; i < 3 * w; i += 3)
		{
			tempStorePos = (3 * w + padding) * j + i;
			outRGBpos = 3 * w * j + i;
			outRGB->data[outRGBpos] = tempStore->data[tempStorePos + 2];
			outRGB->data[outRGBpos + 1] = tempStore->data[tempStorePos + 1];
			outRGB->data[outRGBpos + 2] = tempStore->data[tempStorePos];
		}

	// Set image width and height and allocate storage for image in output RGBA file.
	outRGBA->width = w;
	outRGBA->height = h;
	outRGBA->data = new unsigned char[4 * w * h];

	// Copy image data from output RGB file to output RGBA file, setting all A values to 1.
	for (int j = 0; j < 4 * w * h; j += 4)
	{
		outRGBA->data[j] = outRGB->data[(j / 4) * 3];
		outRGBA->data[j + 1] = outRGB->data[(j / 4) * 3 + 1];
		outRGBA->data[j + 2] = outRGB->data[(j / 4) * 3 + 2];
		outRGBA->data[j + 3] = 0xFF;
	}

	// Release temporary storage and the output RGB file and return the RGBA version.
	delete[] tempStore;
	delete[] outRGB;
	return outRGBA;
}

static unsigned int texture[7]; // Array of texture indices.
static float d = 0.0; // Distance parameter in gluLookAt().

// Load external textures.
void loadTextures()
{
    // Local storage for bmp image data.
	imageFile *image[7];

	// Load the images.
	image[0] = getBMP("pos_x.bmp");
  image[1] = getBMP("negx.bmp");
  image[2] = getBMP("bottom.bmp");
  image[3] = getBMP("top.bmp");
  image[4] = getBMP("pos_z.bmp");
  image[5] = getBMP("negz.bmp");

  image[6] = getBMP("tree_bark.bmp") ;
  //  Bind sky image to texture object texture[i].

for( int i =0;i< 7 ;i++ )
{
  glBindTexture(GL_TEXTURE_2D, texture[i]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[i]->width, image[i]->height, 0,
             GL_RGBA, GL_UNSIGNED_BYTE, image[i]->data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}



}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




     #define PI 3.14159265
     int theta  = 270 ;
    // defines the class camera
    class Camera
    {
    public:
    	Camera();
        double x_coordinate;
        double y_coordinate;
        double z_coordinate;
    };
    // Global camera.
    Camera camera;
    double centerx_coordinate = 40 ;  // -20
    double centery_coordinate = 20 ;   // 10
    double centerz_coordinate = 0 ;   // 0
    Camera::Camera()
    {
        x_coordinate = 0;          // -20
        y_coordinate = 20 ;          // 10
        z_coordinate = 0  ;          // 40
    }

    double influnce_radius_outer = 8 ;
    double  influnce_radius_inner = 0.25 ;
    double alignment_coeff = 8 ;
    double cohesion_coeff =  200 ;
    double seperation_coeff = 2 ;

    // Camera constructor.

    struct vec
    {
      double x =0 ;
      double y =0 ;
      double z =0 ;
      void normalize()
      {
        double denom = x*x +y*y + z*z ;
        denom = sqrt( denom );
        x /= denom ;
        y /= denom ;
        z /= denom ;
      }
      vec( double ax , double ay , double az )
      {
        x = ax , y = ay , z = az ;
      }
    };
//
// void render_texture_on_fish( double x , double y , double z )
// {
//   // glBindTexture(GL_TEXTURE_2D, texture[6]);
//   // glBegin(GL_POLYGON);
//   // glTexCoord2f(0.0, 0.0); glVertex3f(x+0.5, y+0.5, z+0.5) ;
//   // glTexCoord2f(1.0, 0.0); glVertex3f(x+0.5, y+0.5, z-0.5) ;
//   // glTexCoord2f(1.0, 1.0); glVertex3f(x-0.5, y+0.5, z+0.5) ;
//   // glTexCoord2f(0.0, 1.0); glVertex3f(x-0.5, y+0.5, z-0.5) ;
//   // glEnd();
//   //
//   // glBindTexture(GL_TEXTURE_2D, texture[6]);
//   // glBegin(GL_POLYGON);
//   // glTexCoord2f(0.0, 0.0); glVertex3f(x+0.5, y-0.5, z+0.5) ;
//   // glTexCoord2f(1.0, 0.0); glVertex3f(x+0.5, y-0.5, z-0.5) ;
//   // glTexCoord2f(1.0, 1.0); glVertex3f(x-0.5, y-0.5, z+0.5) ;
//   // glTexCoord2f(0.0, 1.0); glVertex3f(x-0.5, y-0.5, z-0.5) ;
//   // glEnd();
//   //
//   // glBindTexture(GL_TEXTURE_2D, texture[6]);
//   // glBegin(GL_POLYGON);
//   // glTexCoord2f(0.0, 0.0); glVertex3f(x+0.5, y+0.5, z+0.5) ;
//   // glTexCoord2f(1.0, 0.0); glVertex3f(x+0.5, y+0.5, z-0.5) ;
//   // glTexCoord2f(1.0, 1.0); glVertex3f(x+0.5, y-0.5, z+0.5) ;
//   // glTexCoord2f(0.0, 1.0); glVertex3f(x+0.5, y-0.5, z-0.5) ;
//   // glEnd();
//   // glBindTexture(GL_TEXTURE_2D, texture[6]);
//   // glBegin(GL_POLYGON);
//   // glTexCoord2f(0.0, 0.0); glVertex3f(x-0.5, y+0.5, z+0.5) ;
//   // glTexCoord2f(1.0, 0.0); glVertex3f(x-0.5, y+0.5, z-0.5) ;
//   // glTexCoord2f(1.0, 1.0); glVertex3f(x-0.5, y-0.5, z+0.5) ;
//   // glTexCoord2f(0.0, 1.0); glVertex3f(x-0.5, y-0.5, z-0.5) ;
//   // glEnd();
//
//   // glBindTexture(GL_TEXTURE_2D, texture[6]);
//   // glBegin(GL_POLYGON);
//   // glTexCoord2f(0.0, 0.0); glVertex3f(x-0.5, y+0.5, z+0.5) ;
//   // glTexCoord2f(1.0, 0.0); glVertex3f(x+0.5, y+0.5, z+0.5) ;
//   // glTexCoord2f(1.0, 1.0); glVertex3f(x-0.5, y-0.5, z+0.5) ;
//   // glTexCoord2f(0.0, 1.0); glVertex3f(x+0.5, y-0.5, z+0.5) ;
//   // glEnd();
//   // glBindTexture(GL_TEXTURE_2D, texture[6]);
//   // glBegin(GL_POLYGON);
//   // glTexCoord2f(0.0, 0.0); glVertex3f(x-0.5, y+0.5, z-0.5) ;
//   // glTexCoord2f(1.0, 0.0); glVertex3f(x+0.5, y+0.5, z-0.5) ;
//   // glTexCoord2f(1.0, 1.0); glVertex3f(x-0.5, y-0.5, z-0.5) ;
//   // glTexCoord2f(0.0, 1.0); glVertex3f(x+0.5, y-0.5, z-0.5) ;
//   // glEnd();
//
//
// }
    struct fish
    {
        vec position  = vec( 10,10,10 ) ;
        vec velocity = vec( 1,1,1 ) ;
        fish( double xc , double yc , double zc , double alxc , double alyc , double alzc  )
        {
            position.x = xc ;
            position.y = yc ;
            position.z = zc ;
            velocity.x = alxc ;
            velocity.y = alyc ;
            velocity.z = alzc ;
        }
        void draw_fish()
        {
          // fish main body
        	glPushMatrix();
          float color[] = { 0 , 0 , 0  } ;
          glColor3f(1,0.35,0)  ;
           glTranslatef( position.x , position. y ,  position.z );
           glScalef(1 , 1 ,1 );
          glutSolidCube(1);
          glPopMatrix();
        }
        bool operator==(const fish& a) const
        {
            return ( position.x == a.position.x &&  position.y == a.position.y &&  position.z == a.position.z &&
                     velocity.x == a.velocity.x &&  velocity.y == a.velocity.y &&  velocity.z == a.velocity.z    ) ;
        }

    } ;

std::vector<fish> b_arr ;







    double distance_b_fish( fish a , fish  b )
    {
      return   sqrt (  (a.position.x-b.position.x)*(a.position.x-b.position.x)    +
                    (a.position.y-b.position.y)*(a.position.y-b.position.y)    +
                    (a.position.z-b.position.z)*(a.position.z-b.position.z)   )   ;

    }

    vec alignment( fish b )
    {
      vec ans = vec( 0,0,0) ;
      int cnt = 0 ;
      for( auto it : b_arr )
      {
        if( distance_b_fish( b , it ) < influnce_radius_outer  )
        {
          cnt ++ ;
          ans.x = ans.x +  it.velocity.x   ;
          ans.y = ans.y +  it.velocity.y   ;
          ans.z = ans.z +  it.velocity.z   ;
        }
      }
        ans.x =   (  (ans.x / cnt) - b.velocity.x ) / alignment_coeff           ;
        ans.y =   (  (ans.y / cnt) - b.velocity.y ) / alignment_coeff           ;
        ans.z =   (  (ans.z / cnt) - b.velocity.z ) / alignment_coeff           ;
    //d    ans.normalize() ;
        return ans ;
    }

    vec cohesion ( fish & b )
    {
      vec ans = vec( 0,0,0) ;
      int cnt = 0 ;
      for( auto it : b_arr )
      {
        if( it == b )
          continue ;

        if( distance_b_fish( b , it ) < influnce_radius_outer  )
        {
          cnt ++ ;
          ans.x = ans.x + it.position.x   ;
          ans.y = ans.y + it.position.y    ;
          ans.z = ans.z + it.position.z    ;
        }
      }
      if( cnt )
      {
        ans.x =   (  (ans.x / cnt) - b.position.x ) / cohesion_coeff           ;
        ans.y =   (  (ans.y / cnt) - b.position.y ) / cohesion_coeff           ;
        ans.z =   (  (ans.z / cnt) - b.position.z ) / cohesion_coeff           ;
      }
      //  ans.normalize() ;
        return ans ;
    }

    vec separation( fish  &b )
    {
      vec ans = vec( 0,0,0) ;
      int cnt = 0 ;
      for( auto it : b_arr )
      {
        if( it == b )
          continue ;
          double dist =  distance_b_fish( b , it ) ;
          double coeff = 10 ;
          dist = dist / coeff ;
          double delchange = 20 ;
        if( dist < influnce_radius_inner  )
        {
          cnt ++ ;
          ans.x = ans.x - (it.position.x -  b.position.x  )   ;
          ans.y = ans.y - (it.position.y -  b.position.y  )   ;
          ans.z = ans.z - (it.position.z -  b.position.z  )   ;
       }
      }
        ans.x = ans.x / seperation_coeff  ;
        ans.y = ans.y / seperation_coeff  ;
        ans.z = ans.z / seperation_coeff  ;
      //  ans.normalize() ;
        return ans ;
    }

    void update_fish_velocity (fish  &b )
    {
      vec sep = separation( b ) ;
      vec coh = cohesion( b )   ;
      vec alg = alignment( b )  ;

      b.velocity.x  = b.velocity.x +   sep.x ;
      b.velocity.y  = b.velocity.y +   sep.y ;
      b.velocity.z  = b.velocity.z +   sep.z ;

      b.velocity.x  = b.velocity.x +   coh.x ;
      b.velocity.y  = b.velocity.y +   coh.y ;
      b.velocity.z  = b.velocity.z +   coh.z ;

      b.velocity.x  = b.velocity.x +   alg.x ;
      b.velocity.y  = b.velocity.y +   alg.y ;
      b.velocity.z  = b.velocity.z +   alg.z ;


      b.velocity.normalize() ;
    }
    void update_fish_loc( double stepsize  , fish  &b   )
    {
      // b.velocity.normalize();
      vec temp  = vec(  b.velocity.x , b.velocity.y , b.velocity.z  ) ;
      temp.normalize() ;
      b.position.x +=  temp.x * stepsize ;
      b.position.y +=  temp.y * stepsize ;
      b.position.z +=  temp.z * stepsize ;

      if( b.position.x >100 || b.position.x < -100 )
        b.velocity.x *= -1 ;
      if( b.position.z >100 || b.position.z < -100 )
        b.velocity.z *= -1 ;
      if( b.position.y >100 || b.position.y < 0 )
        b.velocity.y *= -1 ;

    }
    void helper( int x )
    {
      glutPostRedisplay() ;
    }

    void drawScene(void)
    {
    	  glColor3f(0.0, 0.0, 0.0);
    	  glLoadIdentity();
        //###############    hidden surface removal
        glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA  | GLUT_DEPTH );
        glClear(GL_COLOR_BUFFER_BIT  | GL_DEPTH_BUFFER_BIT );
        glEnable( GL_DEPTH_TEST );

        gluLookAt( camera.x_coordinate , camera.y_coordinate , camera.z_coordinate ,
                 centerx_coordinate  , centery_coordinate  , centerz_coordinate  ,
                 0.0, 1.0, 0.0 );


        /// ---------------------------------------------------------------------------
        //  back
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[0]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(110.0, 0.0, 108.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(-110.0, 0.0, 108.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(-110.0, 150.0, 108.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(110.0, 150.0, 108.0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0 );
        glPopMatrix();
        // front
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[1]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(110.0, 0.0, -108.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(-110.0, 0.0, -108.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(-110.0, 150.0, -108.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(110.0, 150.0, -108.0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0 );
        glPopMatrix();
        // bottom
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[2]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(-115.0, 2.0, 115.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(115.0, 2.0, 115.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(115.0, 2.0, -115.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-115.0, 2.0, -115.0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0 );
        glPopMatrix();
        // //   top
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[3]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(-112.0, 148.0, 112.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(112.0, 148.0, 112.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(112.0, 148.0, -112.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-112.0, 148.0, -112.0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0 );
        glPopMatrix();
        // left
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[4]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(-108.0, 0.0, -110.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(-108.0, 0.0, 110.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(-108.0, 150.0, 110.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(-108.0, 150.0, -110.0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0 );
        glPopMatrix();
        //right
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texture[5]);
        glBegin(GL_POLYGON);
        glTexCoord2f(0.0, 0.0); glVertex3f(108.0, 0.0, -110.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(108.0, 0.0, 110.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(108.0, 150.0, 110.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(108.0, 150.0, -110.0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0 );
        glPopMatrix();
//

       for( auto &it : b_arr )
           {
             update_fish_loc( 0.25 , it  ) ;
             update_fish_velocity( it ) ;
             it.draw_fish() ;
           }

       glutTimerFunc(10 , helper , 1 ) ;
      	glFlush();
}

    // Initialization routine.
    void setup(void)
    {
    	glClearColor(1.0, 1.0, 1.0, 0.0);
      camera = Camera();
/////-----------------------------------------------------------------------------------------------------
      glEnable(GL_DEPTH_TEST);
      // Create texture ids.
      glGenTextures(7, texture);
      // Load external textures.
      loadTextures();
      // Specify how texture values combine with current surface color values.
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      // Turn on OpenGL texturing.
      glEnable(GL_TEXTURE_2D);
/////-----------------------------------------------------------------------------------------------------

    }

    // OpenGL window reshape routine.
    void resize(int w, int h)
    {
    	glViewport(0, 0, w, h);
    	glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();
    //	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 200.0);

    	glFrustum(-1.0, 1.0, -1.0, 1.0, 1, 5000);
    	glMatrixMode(GL_MODELVIEW);
    }

    // Keyboard input processing routine.
    void keyInput(unsigned char key, int x, int y)
    {
    	switch (key)
    	{
    	case 27:
    		exit(0);
    		break;

        // it moves the camera  towards objects
      case 'w' :
        //camera.z_coordinate --  ;
    		camera.x_coordinate += cos( (theta ) *PI / (float )180  ) ;
    		camera.z_coordinate += sin( (theta) *PI / (float )180   );

    		centerx_coordinate = camera.x_coordinate + cos (theta *PI / (float )180 );
    		centerz_coordinate = camera.z_coordinate + sin( theta *PI / (float )180 );
        glutPostRedisplay();
        break;


    // it moves away the camera from objects
      case 's' :
        //camera.z_coordinate ++ ;
    		camera.x_coordinate -= cos( (theta ) *PI / (float )180  ) ;
    		camera.z_coordinate -= sin( (theta) *PI / (float )180   );
    		centerx_coordinate = camera.x_coordinate + cos (theta *PI / (float )180 );
    		centerz_coordinate = camera.z_coordinate + sin( theta *PI / (float )180 );
        glutPostRedisplay();
        break;

    		case 'a' :
    		theta = ( ( theta - 5 )% 360 + 360 )%360 ;
    		centerx_coordinate = camera.x_coordinate + cos (theta *PI / (float )180 );
    		centerz_coordinate = camera.z_coordinate + sin( theta *PI / (float )180 );
    		glutPostRedisplay();
    		break;

    		case 'd':
    		theta = ( ( theta + 5 )% 360 + 360 )%360 ;
    		centerx_coordinate = camera.x_coordinate + cos (theta *PI / (float )180 );
    		centerz_coordinate = camera.z_coordinate + sin( theta *PI / (float )180 );
    		glutPostRedisplay();
    		break;


    	default:
    		break;
    	}
    }


    void specialKeyInput(int key, int x, int y)
    {
    	if (key == GLUT_KEY_DOWN)
      {
        camera.y_coordinate -- ;
        centery_coordinate -- ;
      }
    	if (key == GLUT_KEY_UP)
      {
        camera.y_coordinate ++ ;
        centery_coordinate ++ ;
      }
      if(  key == GLUT_KEY_RIGHT  )
      {
    		camera.x_coordinate += cos( (theta+90) *PI / (float )180  ) ;
    		camera.z_coordinate += sin( (theta+90) *PI / (float )180   );
        centerx_coordinate = camera.x_coordinate + cos (theta *PI / (float )180 );
    		centerz_coordinate = camera.z_coordinate + sin( theta *PI / (float )180 );
      }
      if(  key == GLUT_KEY_LEFT  )
      {
        camera.x_coordinate -= cos( (theta+90) *PI / (float )180  ) ;
    		camera.z_coordinate -= sin( (theta+90) *PI / (float )180   );
        centerx_coordinate = camera.x_coordinate + cos (theta *PI / (float )180 );
    		centerz_coordinate = camera.z_coordinate + sin( theta *PI / (float )180 );
      }
    	glutPostRedisplay();
    }

    // Main routine.
    int main(int argc, char **argv)
    {




     for(int i = 0 ;i < 300 ;i++ )
     {
       int x = rand() %100  ;
       int z =  rand()%100  ;
       int y =  rand()%100  ;
       int alx = rand()%100  ;
       int alz = rand()%100  ;
       int aly = rand()%100  ;
       b_arr.emplace_back( fish(x,y,z,alx,aly,alz)   ) ;
     }
    	glutInit(&argc, argv);
    	glutInitContextVersion(4, 3);
    	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    	// glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);


      //$$$$$$$$$$$$$$$$$$
      glutInitDisplayMode( GLUT_SINGLE | GLUT_RGBA  | GLUT_DEPTH );
      glEnable( GL_DEPTH_TEST );
      cout<< "INTERACTION \n press w for zoom in \n press s for zoom out \n";
      cout<<" press a for turning camera in anticlockwise direction\n ";
      cout<<"press d for turning camera in  clockwise direction\n " ;
      cout<< "press left arrow for moving left \n";
      cout<< "press right arrow for moving right \n";
      cout<< "press up arrow for moving up \n";
      cout<< "press down arrow for down left \n";
      //$$$$$$$$$$$$$$$$$$$$$$$$
    	glutInitWindowSize(1000, 1000);
    	glutInitWindowPosition(100, 100);
    	glutCreateWindow("box.cpp");
    	glutDisplayFunc(drawScene);
    	glutReshapeFunc(resize);
    	glutKeyboardFunc(keyInput);
      glutSpecialFunc(specialKeyInput);
    	glewExperimental = GL_TRUE;
    	glewInit();
    	setup();
    	glutMainLoop();
    }
