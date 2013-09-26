/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager.field;

import bica.Arrow;
import bica.Circle;
import bica.Ellipse;
import bica.Line;
import bica.Rectangle;
import java.awt.Color;
import jmanager.*;
import java.util.ArrayList;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import javax.media.opengl.GLEventListener;
import javax.media.opengl.glu.GLU;
import javax.media.opengl.glu.GLUquadric;

/**
 *
 * @author Eduardo Perdices
 */
public class GLFieldEvent implements GLEventListener {

	private long lastit;
	private ArrayList<FieldLine> lines;
	private ArrayList<FieldCircle> circles;
	private String colorplayer;
	private float escale;
	private boolean isInitField;
	private float camx,  camy,  camz;
	private float lookx,  looky,  lookz;
	private float mousex,  mousey;
	private float projectionx,  projectiony;
        private bica.Shape[] shapeList = null;

	protected void drawOpenGL(GLAutoDrawable drawable){}

	public void init(GLAutoDrawable drawable) {
		GL gl = drawable.getGL();
		System.out.println("INIT GL IS: " + gl.getClass().getName());

		// Enable VSync
		gl.setSwapInterval(1);

		// Setup the drawing area and shading mode
		gl.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		gl.glShadeModel(GL.GL_SMOOTH); // try setting this to GL_FLAT and see what happens.

		this.lastit = 0;

		//Init parameters
		this.isInitField = true;
		this.escale = (float) 1000.0;
		this.camx = -1.0f;
		this.camy = 0.0f;
		this.camz = 8.5f;
		this.lookx = this.looky = this.lookz = 0.0f;
	}

	public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
		GL gl = drawable.getGL();
		GLU glu = new GLU();

		if (height <= 0) { // avoid a divide by zero error!

			height = 1;
		}
		final float h = (float) width / (float) height;
		gl.glViewport(0, 0, width, height);
		gl.glMatrixMode(GL.GL_PROJECTION);
		gl.glLoadIdentity();
		/*                  Angulo	ratio	znear,  zfar*/
		glu.gluPerspective(50.0f, h, 1.0, 100.0);
		gl.glMatrixMode(GL.GL_MODELVIEW);
		gl.glLoadIdentity();
	}

	public void display(GLAutoDrawable drawable) {

            //System.out.println("*display");
           
		if(!this.displayField())
			return;
            

		GL gl = drawable.getGL();
		GLU glu = new GLU();
                gl.glEnable (gl.GL_BLEND);
                gl.glBlendFunc (gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA);


		//Color de fondo
		gl.glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

		gl.glClearDepth(1.0f);
		gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
		gl.glEnable(gl.GL_DEPTH_TEST);

		// Reset the current matrix to the "identity"
		gl.glLoadIdentity();

		/*pos cam                  pto central         vector up*/
		glu.gluLookAt(this.camx, this.camy, this.camz, this.lookx, this.looky, this.lookz, 0.0f, 0.0f, 1.0f);

		if (this.isInitField) {
			//Draw RoboCup Field
			drawField(gl, glu);
		}

                DrawDebugAbs(gl, glu);


		//Draw owngl.glColor3f(1.0f, 1.0f, 1.0f); implementation
		this.drawOpenGL(drawable);

		/*Calculate projection of clicks*/
		this.calculateProjection(drawable);

		// Flush all drawing operations to the graphics card
		gl.glFlush();


	}

	public void displayChanged(GLAutoDrawable drawable, boolean modeChanged, boolean deviceChanged) {
	}

	public void updateCameraPosition(float x, float y, float z) {
		this.camx += x;
		this.camy += y;
		this.camz += z;
		this.lookx += x;
		this.looky += y;
	}

	public void setProjection(float x, float y) {
		this.mousex = x;
		this.mousey = y;
	}

	public float[] getProjection() {
		float fv[] = {this.projectionx, this.projectiony};
		return fv;
	}

	public void initField() {

		FieldPoint p1, p2, p3, p4, p5, p6, p7, p8;
		FieldLine l1, l2, l3, l4;

		if (this.isInitField) {
			return;
		}

		this.isInitField = true;
		this.lines = new ArrayList<FieldLine>();
		this.circles = new ArrayList<FieldCircle>();

		//Obtain the team color		
		//dataResp respplayer = JmanagerView.root.getConnection().send("getColorPlayer:");
		//this.colorplayer = new String(respplayer.data);
                this.colorplayer = "";
                if (JmanagerView.root.getConnection() != null) {
                    this.colorplayer = JmanagerView.root.getConnection().teamInfoPrx.getTeamColor();
                }
                //System.out.println(colorplayer);
		
		//Obtain world model
		//dataResp resp = JmanagerView.root.getConnection().send("getWorldModel:");
		//String sreceived = new String(resp.data);
                String sreceived = "";
                if (JmanagerView.root.getConnection() != null) {
                    sreceived = JmanagerView.root.getConnection().worldModelPrx.getWorldModel();
                }

		try {
			String[] objects = sreceived.split("#");
			for (int i = 1; i < objects.length; i++) {
				String[] params = objects[i].split(":");
				for (int j = 0; j < params.length; j++) {
					if (params[0].equals("L")) {
						/*New line*/
						this.lines.add(this.getFieldLine(params));
					} else if (params[0].equals("C")) {
						/*New circle*/
						this.circles.add(this.getFieldCircle(params));
					} else {
						System.err.println("(GLField) WorldModel unknown");
					}
				}
			}
		} catch (NumberFormatException e) {
			System.err.println("(GLField) Data error: Number format not valid");
		} catch (Exception e) {
			System.err.println("(GLField) Data error: Unknown exception");
			System.err.println(e);
		}
	}

	private boolean displayField() {

		//if(!this.isInitField)
		//	return false;

                //System.out.println(".");
		/*Display every 250 ms*/
		if(System.currentTimeMillis()-this.lastit < 250)
			return false;

		this.lastit = System.currentTimeMillis();

		return true;
	}

	private void calculateProjection(GLAutoDrawable drawable) {

		if (!this.isInitField)
			return;

		GL gl = drawable.getGL();
		GLU glu = new GLU();

		int viewport[] = new int[4];
		double mvmatrix[] = new double[16];
		double projmatrix[] = new double[16];
		double wcoord[] = new double[4];
		float realy;

		gl.glGetDoublev(GL.GL_MODELVIEW_MATRIX, mvmatrix, 0);
		gl.glGetDoublev(GL.GL_PROJECTION_MATRIX, projmatrix, 0);
		gl.glGetIntegerv(GL.GL_VIEWPORT, viewport, 0);

		realy = viewport[3] - (int) this.mousey - 1;

		glu.gluUnProject((double) this.mousex, (double) realy, 0.0, mvmatrix, 0, projmatrix, 0, viewport, 0, wcoord, 0);

		this.projectionx = (float) wcoord[0];
		this.projectiony = (float) wcoord[1];

		//System.out.println("Hemos obtenido de la proyección " + wcoord[0] + " " + wcoord[1] + " " + wcoord[2]);
	}

	private FieldLine getFieldLine(String params[]) {
		FieldPoint p1, p2, p3, p4;
		float player;

		if(this.colorplayer.equalsIgnoreCase("blue"))
			player = -1.0f;
		else
			player = 1.0f;

		try {
			p1 = new FieldPoint(Float.parseFloat(params[1])*player, Float.parseFloat(params[2])*player,
					Float.parseFloat(params[3]), Float.parseFloat(params[4]));
			p2 = new FieldPoint(Float.parseFloat(params[5])*player, Float.parseFloat(params[6])*player,
					Float.parseFloat(params[7]), Float.parseFloat(params[8]));
			p3 = new FieldPoint(Float.parseFloat(params[9])*player, Float.parseFloat(params[10])*player,
					Float.parseFloat(params[11]), Float.parseFloat(params[12]));
			p4 = new FieldPoint(Float.parseFloat(params[13])*player, Float.parseFloat(params[14])*player,
					Float.parseFloat(params[15]), Float.parseFloat(params[16]));

			return new FieldLine(p1, p2, p3, p4, p1, p2, p3, p4, params[17]);
		} catch (NumberFormatException e) {
			System.err.println("(GLField) WorldModel: Data error");
		}

		return null;
	}

	private FieldCircle getFieldCircle(String params[]) {
		FieldPoint p1, p2;
		float r1, r2;
		float player;

		if(this.colorplayer.equalsIgnoreCase("blue"))
			player = -1.0f;
		else
			player = 1.0f;

		try {
			p1 = new FieldPoint(Float.parseFloat(params[1])*player, Float.parseFloat(params[2])*player,
					Float.parseFloat(params[3]), Float.parseFloat(params[4]));
			p2 = new FieldPoint(Float.parseFloat(params[5])*player, Float.parseFloat(params[6])*player,
					Float.parseFloat(params[7]), Float.parseFloat(params[8]));
			r1 = Float.parseFloat(params[9]);
			r2 = Float.parseFloat(params[10]);

			return new FieldCircle(p1, p2, r1, r2, params[11]);
		} catch (NumberFormatException e) {
			System.err.println("(GLField) WorldModel: Data error");
		}

		return null;
	}

	private void drawField(GL gl, GLU glu) {

		float height;

		GLUquadric cylinder = glu.gluNewQuadric();
		glu.gluQuadricDrawStyle(cylinder, GLU.GLU_LINE);

		gl.glBegin(GL.GL_QUADS);
		for (int i = 0; i < this.lines.size(); i++) {
			FieldLine fl = this.lines.get(i);
			if (fl.getColor().equals("WHITE")) {
				gl.glColor3f(1.0f, 1.0f, 1.0f);
			} else if (fl.getColor().equals("BLUE")) {
				gl.glColor3f(0.0f, 0.0f, 1.0f);
			} else if (fl.getColor().equals("YELLOW")) {
				gl.glColor3f(1.0f, 1.0f, 0.0f);
			} else
				gl.glColor3f(1.0f, 1.0f, 1.0f);

			gl.glVertex3f(fl.getP1().getX() / this.escale, fl.getP1().getY() / this.escale, fl.getP1().getZ() / this.escale);
			gl.glVertex3f(fl.getP2().getX() / this.escale, fl.getP2().getY() / this.escale, fl.getP2().getZ() / this.escale);
			gl.glVertex3f(fl.getP4().getX() / this.escale, fl.getP4().getY() / this.escale, fl.getP4().getZ() / this.escale);
			gl.glVertex3f(fl.getP3().getX() / this.escale, fl.getP3().getY() / this.escale, fl.getP3().getZ() / this.escale);
		}
		gl.glEnd();

		for (int i = 0; i < this.circles.size(); i++) {
			FieldCircle fc = this.circles.get(i);
			if (fc.getColor().equals("WHITE")) {
				gl.glColor3f(1.0f, 1.0f, 1.0f);
			} else if (fc.getColor().equals("BLUE")) {
				gl.glColor3f(0.0f, 0.0f, 1.0f);
			} else if (fc.getColor().equals("YELLOW")) {
				gl.glColor3f(1.0f, 1.0f, 0.0f);
			}

			gl.glPushMatrix();

			gl.glTranslatef(fc.getP1().getX() / this.escale, fc.getP1().getY() / this.escale, fc.getP1().getZ() / this.escale);

			/*Cylinder*/
			if (fc.getR2() == 0.0) {
				/*Goal posts circles*/
				height = Math.abs(fc.getP2().getZ() - fc.getP1().getZ());

				/*Check de axis*/
				if (fc.getP1().getY() != fc.getP2().getY()) {
					/*Vertical post*/
					if(this.colorplayer.equalsIgnoreCase("blue"))
						gl.glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
					else
						gl.glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
					height = Math.abs(fc.getP2().getY() - fc.getP1().getY());
				}

				glu.gluCylinder(cylinder, fc.getR1() / this.escale, fc.getR1() / this.escale, height / this.escale, 50, 20);
			} else {
				/*Central circle*/
				glu.gluDisk(cylinder, fc.getR2() / this.escale, fc.getR1() / this.escale, 50, 20);
			}

			gl.glPopMatrix();
		}
	}

 
    public void SetDebugAbs(bica.Shape[] shapeListArg) {

        shapeList = shapeListArg;
    }

    private void DrawDebugAbs(GL gl, GLU glu) {

        if(shapeList == null)
           return;
         
          for (int i = 0; i < shapeList.length; i++) {
                   bica.Shape shape = shapeList[i];
                Color color;

                switch (shape.color) {
                    case BLUE: color = Color.BLUE; break;
                    case GREEN: color = Color.GREEN; break;
                    case RED: color = Color.RED; break;
                    case ORANGE: color = Color.ORANGE; break;
                    case PURPLE: color = Color.MAGENTA; break;
                    case GREY: color = Color.GRAY; break;
                    case YELLOW: color = Color.YELLOW; break;
                    case BLACK: color = Color.BLACK; break;
                    case WHITE: color = Color.WHITE; break;
                    default: color = Color.BLACK; break;
                }

                
                try{
                	Color col = new Color(color.getRed(), color.getGreen(), color.getBlue(), shape.opacity);

                	if (shape instanceof bica.Circle) {
                        drawCircle((bica.Circle)shape, col, shape.filled, shape.label, gl, glu);
                    } else if (shape instanceof bica.Line ) {
                        drawLine((bica.Line)shape, col, shape.label, gl, glu);
                    } else if (shape instanceof bica.Ellipse) {
                        drawEllipse((bica.Ellipse)shape, col, shape.filled, shape.label, gl, glu);
                    } else if (shape instanceof bica.Arrow) {
                        drawArrow((bica.Arrow)shape, col, shape.label, gl, glu);
                    } else if (shape instanceof bica.Rectangle) {
                        drawRectangle((bica.Rectangle)shape, col, shape.label, gl, glu);
                   } else
                        System.out.println("Object unknown\n");

                }catch (Exception ex)
                {
                	System.err.println("============Error :"+shape.opacity);
                }
                
             }
    }

    private void drawLine(Line line, Color col, String label, GL gl, GLU glu) {


        gl.glPushMatrix();
        gl.glLineWidth(line.width/ this.escale);
        gl.glBegin(gl.GL_LINES);

        gl.glColor4f(((float)col.getRed())/255.0f, ((float)col.getGreen())/255.0f, ((float)col.getBlue())/255.0f, ((float)col.getAlpha())/255.0f);
        gl.glVertex3f(line.p1.x / this.escale, line.p1.y / this.escale, line.p1.z / this.escale);
        gl.glVertex3f(line.p2.x / this.escale, line.p2.y / this.escale, line.p2.z / this.escale);

        gl.glEnd();
        gl.glPopMatrix();
    }

    private void drawCircle(Circle circle, Color col, boolean filled, String label, GL gl, GLU glu) {

        float x, y, z, radius;

        x = circle.center.x/ this.escale;
        y = circle.center.y/ this.escale;
        z = circle.center.z/ this.escale;
        radius =  circle.radius/ this.escale;

        gl.glPushMatrix();
	
        gl.glColor4f(((float)col.getRed())/255.0f, ((float)col.getGreen())/255.0f, ((float)col.getBlue())/255.0f, ((float)col.getAlpha())/255.0f);
	gl.glLineWidth(1.0f);
	gl.glBegin(gl.GL_POLYGON);
	float xmod, ymod;
 
        for(float theta = 0.0f; theta <= 2*Math.PI; theta += 0.1)
	{
		xmod = (float) (Math.cos(theta) * radius);
		ymod = (float) (Math.sin(theta) * radius);
		gl.glVertex3f(x + xmod, y + ymod, z); //create solids by vertices
	}
	gl.glEnd();
	gl.glPopMatrix();
    }

    private void drawEllipse(Ellipse ellipse, Color col, boolean filled, String label, GL gl, GLU glu) {


        float q = ellipse.angle;


        gl.glPushMatrix();
        gl.glColor4f(((float)col.getRed())/255.0f, ((float)col.getGreen())/255.0f, ((float)col.getBlue())/255.0f, ((float)col.getAlpha())/255.0f);
	gl.glLineWidth(1.0f);

        gl.glBegin(gl.GL_POLYGON);

        for (double a = -Math.PI; a < Math.PI; a = a + 0.05) {
            //convert degrees into radians
            float px, py, ex, ey;

            ex = (float) (Math.cos(a) * (ellipse.width/ this.escale));
            ey = (float) (Math.sin(a) * (ellipse.length/ this.escale));

            px = (float) (ex * Math.cos(ellipse.angle) - ey * Math.sin(ellipse.angle));
            py = (float) (ex * Math.sin(ellipse.angle) + ey * Math.cos(ellipse.angle));

            px = px + ellipse.center.x/ this.escale;
            py = py + ellipse.center.y/ this.escale;

            gl.glVertex3f(px, py, ellipse.center.z/ this.escale);
        }

        gl.glEnd();
        gl.glPopMatrix();
    }

    private void drawArrow(Arrow arrow, Color col, String label, GL gl, GLU glu) {
 
        float dist = 200.0f;
        double ang = Math.atan2(arrow.src.y - arrow.dst.y , arrow.src.x - arrow.dst.x);

        gl.glPushMatrix();
        gl.glLineWidth(arrow.width/ this.escale);
        gl.glBegin(gl.GL_LINES);

        gl.glColor4f(((float)col.getRed())/255.0f, ((float)col.getGreen())/255.0f, ((float)col.getBlue())/255.0f, ((float)col.getAlpha())/255.0f);
        gl.glVertex3f(arrow.src.x / this.escale, arrow.src.y / this.escale, arrow.src.z / this.escale);
        gl.glVertex3f(arrow.dst.x / this.escale, arrow.dst.y / this.escale, arrow.dst.z / this.escale);
        gl.glVertex3f((float)(arrow.dst.x + dist * 0.75 * Math.cos(ang + 0.3))/ this.escale, (float)(arrow.dst.y + dist * 0.75 * Math.sin(ang + 0.3))/ this.escale, arrow.dst.z / this.escale);
        gl.glVertex3f(arrow.dst.x / this.escale, arrow.dst.y / this.escale, arrow.dst.z / this.escale);
        gl.glVertex3f((float)(arrow.dst.x + dist * 0.75 * Math.cos(ang - 0.3))/ this.escale, (float)(arrow.dst.y + dist * 0.75 * Math.sin(ang - 0.3))/ this.escale, arrow.dst.z / this.escale);
        gl.glVertex3f(arrow.dst.x / this.escale, arrow.dst.y / this.escale, arrow.dst.z / this.escale);
        gl.glEnd();
        gl.glPopMatrix();


    }

    private void drawRectangle(Rectangle rectangle, Color col, String label, GL gl, GLU glu) {
        gl.glPushMatrix();

        gl.glColor4f(((float)col.getRed())/255.0f, ((float)col.getGreen())/255.0f, ((float)col.getBlue())/255.0f, ((float)col.getAlpha())/255.0f);
	gl.glLineWidth(1.0f);
	gl.glBegin(gl.GL_POLYGON);

        float x1,x2,x3,x4,y1,y2,y3,y4,z;

        z = rectangle.p1.z/ this.escale;

        x1 = rectangle.p1.x/ this.escale;
        y1 = rectangle.p1.y/ this.escale;

        x2 = rectangle.p1.x/ this.escale;
        y2 = rectangle.p2.y/ this.escale;

        x3 = rectangle.p2.x/ this.escale;
        y3 = rectangle.p2.y/ this.escale;

        x4 = rectangle.p2.x/ this.escale;
        y4 = rectangle.p1.y/ this.escale;

        gl.glVertex3f(x1,y1,z);
        gl.glVertex3f(x2,y2,z);
        gl.glVertex3f(x3,y3,z);
        gl.glVertex3f(x4,y4,z);
 	gl.glEnd();
	gl.glPopMatrix();
    }

}
