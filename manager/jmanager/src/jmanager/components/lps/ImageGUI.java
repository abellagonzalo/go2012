package jmanager.components.lps;

import java.awt.Color;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Polygon;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.geom.AffineTransform;
import java.awt.geom.Line2D;
import java.awt.geom.Point2D;
import java.util.Observable;
import java.util.Observer;
import java.util.Random;
import java.util.Vector;
import jmanager.GrDebug.*;

public class ImageGUI extends javax.swing.JPanel implements Observer, KeyListener {

    /**
     *
     */
    private static final long serialVersionUID = 3850595976794268700L;
    //OBSERVABLES
    //private LPS kalmanLps = null;
    private GrPrimitivesRel grPrimitives = null;
    bica.Shape[] shapeList;
    KeyMask keymask;
    //IMAGE VALUES
    private Graphics2D g;
    private final int PARTICLE_SIZE = 3;
    private final int IMAGE_SIZE = 300;
    private int MAX_VISUAL_DIST = 5000;
    private final int BALL_SIZE = 20;
    //private final int BALL_SIZE = 8 * IMAGE_SIZE / MAX_VISUAL_DIST;
    private final int NET_SIZE = 40 * IMAGE_SIZE / MAX_VISUAL_DIST;

    AffineTransform tx;
    Line2D.Double line;
    Polygon arrowHead;

    public ImageGUI(GrPrimitivesRel grPrimitives) {

        this.grPrimitives = grPrimitives;
        shapeList = new bica.Shape[1];
        keymask = new KeyMask();

        this.setBackground(new Color(35, 120, 25));

        this.setMaximumSize(new java.awt.Dimension(300, 300));
        this.setPreferredSize(new java.awt.Dimension(300, 300));

        javax.swing.GroupLayout imageLayout = new javax.swing.GroupLayout(this);
        this.setLayout(imageLayout);
        imageLayout.setHorizontalGroup(
                imageLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING).addGap(0, 300, Short.MAX_VALUE));
        imageLayout.setVerticalGroup(
                imageLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING).addGap(0, 300, Short.MAX_VALUE));

        // create an AffineTransform 
        // and a triangle centered on (0,0) and pointing downward
        // somewhere outside Swing's paint loop
        tx = new AffineTransform();
        line = new Line2D.Double(0,0,100,100);

        arrowHead = new Polygon();  
        arrowHead.addPoint( 0,5);
        arrowHead.addPoint( -5, -5);
        arrowHead.addPoint( 5,-5);

        this.setFocusable(true);
        this.addKeyListener(this);
    }

    @Override
    public void paint(Graphics gr) {
        super.paint(gr);
        g = (Graphics2D) gr;
        g.setPaint(Color.BLACK);

        this.requestFocus();

        paintWords();
        paintCoordenateAxes();
        /*System.out.println(distance2PixelX(2500));
        System.out.println(distance2PixelY(2500));
        System.out.print("Ancho: ");
        System.out.println(ImageGUI.WIDTH);
        System.out.print(" alto: ");
        System.out.println(ImageGUI.HEIGHT);*/
        
        //calculateEllipse(2500, 2500, 800, 200, 45.0f, Color.ORANGE);
        //paintBall();
        //calculateEllipse(200, 200, 20, 40, 135, Color.BLACK);
        //myDrawCircle(100, 100, 40, Color.ORANGE);
//        paintBallBounds();

//        paintBlueNet();
//        paintBlueNetBounds();

        for (int i = 1; i < shapeList.length; i++) {
                bica.Shape shape = shapeList[i];

                // Only display if the associated accelerated key is On
                if (!keymask.isActive(shape.accKey))
                    continue;

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

                Color col = new Color(color.getRed(), color.getGreen(), color.getBlue(), shape.opacity);

                if (shape instanceof bica.Circle) {
                    drawCircle((bica.Circle)shape, col, shape.filled, shape.label);
                } else if (shape instanceof bica.Line) {
                    drawLine((bica.Line)shape, col, shape.label);
                } else if (shape instanceof bica.Ellipse) {
                    drawEllipse((bica.Ellipse)shape, col, shape.filled, shape.label);
                } else if (shape instanceof bica.Arrow) {
                    drawArrow((bica.Arrow)shape, col, shape.label);
                } else if (shape instanceof bica.Rectangle) {
                    drawRect((bica.Rectangle)shape, col, shape.filled, shape.label);
                } else
                    System.out.println("Object unknown\n");
            }
    }

    @Override
    public void repaint() {
        super.repaint();
    }

     /** Handle the key-released event from the text field. */
    public void keyReleased(KeyEvent e) {
        keymask.keyPressed(e.getKeyChar());

    }

    ///////////////////////////////////////////////////////////////////////////////////////
    private int distance2PixelX(double distance) {

        //return (int) ((-distance + MAX_VISUAL_DIST) * IMAGE_SIZE) / (2 * MAX_VISUAL_DIST);
        return (int)((IMAGE_SIZE / 2) + ((distance * IMAGE_SIZE) / (MAX_VISUAL_DIST * 2)));
    }

    private int distance2PixelY(double distance) {
        return (int)((IMAGE_SIZE / 2) - ((distance * IMAGE_SIZE) / (MAX_VISUAL_DIST * 2)));
        //return (int) ((-distance + MAX_VISUAL_DIST) * IMAGE_SIZE) / (2 * MAX_VISUAL_DIST);
    }

    /////////////////////////////////////////////////////////////////////////////////////////
    /*
* This functions returns an array containing 36 points to draw an
* ellipse.
*
* @param x {double} X coordinate
* @param y {double} Y coordinate
* @param a {double} Semimajor axis
* @param b {double} Semiminor axis
* @param angle {double} Angle of the ellipse
*/
private void calculateEllipse(int x, int y, int a, int b, float angle, Color color, Boolean filled, String label)
{
  x = distance2PixelX(x);
  y = distance2PixelY(y);
  a = a * IMAGE_SIZE / (MAX_VISUAL_DIST * 2);
  b = b * IMAGE_SIZE / (MAX_VISUAL_DIST * 2);

  // Number of points for the ellipse
  int steps = 36;
  Polygon polygon = new Polygon();

  // Angle is given by Degree Value
  double beta = -angle * (Math.PI / 180); //(Math.PI/180) converts Degree Value into Radians
  double sinbeta = Math.sin(beta);
  double cosbeta = Math.cos(beta);

  for (int i = 0; i < 360; i += 360 / steps)
  {
    double alpha = i * (Math.PI / 180) ;
    double sinalpha = Math.sin(alpha);
    double cosalpha = Math.cos(alpha);

    double X = x + (a / 2 * cosalpha * cosbeta - b / 2 * sinalpha * sinbeta);
    double Y = y + (a / 2 * cosalpha * sinbeta + b / 2 * sinalpha * cosbeta);

    polygon.addPoint((int)X, (int)Y);
   }

  Color aux = (Color) g.getPaint();
  g.setPaint(color);

  if (filled) {
      g.fillPolygon(polygon);
  } else {
      g.drawPolygon(polygon);
  }

  g.setPaint(aux);

  g.drawString(label, x + 2, y + 2);

}


    private void myDrawCircle(int x, int y, int radio, Color c, Boolean filled, String label) {
        Color aux = (Color) g.getPaint();
        g.setPaint(c);
        if (filled)
            g.fillOval(x, y, radio, radio);
        else
            g.drawOval(x, y, radio, radio);
        g.setPaint(aux);
        g.drawString(label, x + 2, y + 2);
    }

    private void myDrawLine(int arg0, int arg1, int arg2, int arg3, Color c, String label) {
        Color aux = (Color) g.getPaint();
        g.setPaint(c);
        g.drawLine(arg0, arg1, arg2, arg3);
        g.setPaint(aux);
        g.drawString(label, ((arg0 + arg2) / 2) + 2, ((arg1 + arg3) / 2) + 2);
    }

    private void myDrawArrow(int arg0, int arg1, int arg2, int arg3, Color c, String label) {
        Color aux = (Color) g.getPaint();
        g.setPaint(c);

        myDrawLine(arg0, arg1, arg2, arg3, c, label);

        // Arrow
        tx.setToIdentity();
        double angle = Math.atan2(arg3-arg1, arg2-arg0);
        tx.translate(arg2, arg3);
        tx.rotate((angle-Math.PI/2d));
        Graphics2D gr = (Graphics2D) g.create();
        gr.setTransform(tx);
        gr.fill(arrowHead);
        gr.dispose();

        g.setPaint(aux);
        g.drawString(label, ((arg0 + arg2) / 2) + 2, ((arg1 + arg3) / 2) + 2);
    }

    private void myDrawRect(int x, int y, int w, int h, Color c, Boolean filled, String label) {
        Color aux = (Color) g.getPaint();
        g.setPaint(c);
        if (filled)
            g.fillRect(x, y, w, h);
        else
            g.drawRect(x, y, w, h);
        g.setPaint(aux);
        g.drawString(label, x + w / 2, y + h / 2);
    }

    private void myDrawString(String s, int i, int j, Font f, Color c) {
        Color aux = (Color) g.getPaint();
        g.setPaint(c);
        g.setFont(f);
        g.drawString(s, i, j);
        g.setPaint(aux);
    }

    private void myDrawParticle(int x, int y, Color c) {
        myDrawCircle(x - (PARTICLE_SIZE / 2), y - (PARTICLE_SIZE / 2), PARTICLE_SIZE, c, true, "");
    }

    private void myDrawBall(int x, int y, Color color, Boolean filled) {
        myDrawCircle(x - (BALL_SIZE / 2), y - (BALL_SIZE / 2), BALL_SIZE, color, filled, "");
    }

    private void myDrawBlueNet(int x, int y) {
        myDrawCircle(x - (BALL_SIZE / 2), y - (BALL_SIZE / 2), BALL_SIZE, Color.BLUE, true, "");
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    private void paintCoordenateAxes() {

        for (int i = 0; i <= this.IMAGE_SIZE; i = i + 30) {
            this.myDrawLine(0, i, this.IMAGE_SIZE, i, Color.GRAY, "");
            this.myDrawLine(i, 0, i, this.IMAGE_SIZE, Color.GRAY, "");
        }

        this.myDrawLine(0, this.IMAGE_SIZE / 2, this.IMAGE_SIZE, (this.IMAGE_SIZE / 2), Color.BLACK, "");
        this.myDrawLine((this.IMAGE_SIZE / 2), 0, (this.IMAGE_SIZE / 2), this.IMAGE_SIZE, Color.BLACK, "");
    }

    private void paintBall() {
        //this.myDrawBall(distance2PixelY(kalmanLps.getLPO(kalmanLps.BALL).getY()), distance2PixelX(kalmanLps.getLPO(kalmanLps.BALL).getX()));        
        myDrawBall(300 , 300, Color.ORANGE, true);
    }

/*    private void paintBlueNet() {
        this.myDrawBlueNet(distance2PixelY(kalmanLps.getLPO(kalmanLps.BLUENET).getY()), distance2PixelX(kalmanLps.getLPO(kalmanLps.BLUENET).getX()));
    }

    private void paintBallBounds() {
        double dx = kalmanLps.getLPO(0).getDx();
        double dy = kalmanLps.getLPO(0).getDy();
        float ballX = distance2PixelY(kalmanLps.getLPO(kalmanLps.BALL).getY());
        float ballY = distance2PixelX(kalmanLps.getLPO(kalmanLps.BALL).getX());
        int widthBound = (int) (dy * IMAGE_SIZE / MAX_VISUAL_DIST);
        int heightBound = (int) (dx * IMAGE_SIZE / MAX_VISUAL_DIST);
        myDrawRect((int) (ballX - (widthBound / 2)), (int) (ballY - (heightBound / 2)), widthBound, heightBound, Color.ORANGE);
    }

    private void paintBlueNetBounds() {
        double dx = kalmanLps.getLPO(1).getDx();
        double dy = kalmanLps.getLPO(1).getDy();
        float blueNetX = distance2PixelY(kalmanLps.getLPO(kalmanLps.BLUENET).getY());
        float blueNetY = distance2PixelX(kalmanLps.getLPO(kalmanLps.BLUENET).getX());
        int widthBound = (int) (dy * IMAGE_SIZE / MAX_VISUAL_DIST);
        int heightBound = (int) (dx * IMAGE_SIZE / MAX_VISUAL_DIST);
        myDrawRect((int) (blueNetX - (widthBound / 2)), (int) (blueNetY - (heightBound / 2)), widthBound, heightBound, Color.BLUE);
    }*/

    private void drawCircle (bica.Circle circle, Color color, Boolean filled, String label) {
        //System.out.println("ImageGui::drawCircle()");
        myDrawCircle((int)(distance2PixelX((int)-circle.center.y - circle.radius)),
                (int)(distance2PixelY((int)circle.center.x + circle.radius)),
                (int)((circle.radius * 2 * IMAGE_SIZE) / (MAX_VISUAL_DIST * 2)), color, filled, label);
    }

    private void drawLine (bica.Line line, Color color, String label) {
        myDrawLine(distance2PixelX((int)-line.p1.y), distance2PixelY((int)line.p1.x),
                distance2PixelX((int)-line.p2.y), distance2PixelY((int)line.p2.x),
                color, label);
    }

    private void drawArrow (bica.Arrow arrow, Color color, String label) {
        myDrawArrow(distance2PixelX((int)-arrow.src.y), distance2PixelY((int)arrow.src.x),
                distance2PixelX((int)-arrow.dst.y), distance2PixelY((int)arrow.dst.x),
                color, label);
    }

    private void drawEllipse(bica.Ellipse ellipse, Color color, Boolean filled, String label) {
        calculateEllipse((int)-ellipse.center.y, (int)ellipse.center.x,
                (int)ellipse.length, (int)ellipse.width, 90 + ellipse.angle,
                color, filled, label);
    }

    private void drawRect (bica.Rectangle rect, Color color, Boolean filled, String label) {
        //The point always will be the left point
        int x, y, width, height;
        int x1 = distance2PixelX((int)-rect.p1.y);
        int y1 = distance2PixelY((int)rect.p1.x);
        int x2 = distance2PixelX((int)-rect.p2.y);
        int y2 = distance2PixelY((int)rect.p2.x);

        if (rect.p1.y < rect.p2.y) {
           if (rect.p1.x < rect.p2.x) {
            x = x2;
            y = y2;
            width = x1 - x2;
            height = y1 - y2;
           } else {
            x = x2;
            y = y1;
            width = x1 - x2;
            height = y2 - y1;
           }
        } else {
           if (rect.p1.x < rect.p2.x) {
            x = x1;
            y = y2;
            width = x2 - x1;
            height = y1 - y2;
           } else {
            x = x1;
            y = y1;
            width = x2 - x1;
            height = y2 - y1;
           }
        }

        myDrawRect(x, y, width, height, color, filled, label);
    }

    private void paintWords() {
        Font font = new Font("Arial", Font.TRUETYPE_FONT, 10);
        int alto, ancho;
        alto = MAX_VISUAL_DIST;
        ancho = MAX_VISUAL_DIST;

        // - +
        String s = "[" + ancho + ", " + alto + "]";
        myDrawString(s, 2, 10, font, Color.BLACK);
        // + +
        s = "[" + ancho + ", -" + alto + "]";
        myDrawString(s, this.IMAGE_SIZE - 62, 10, font, Color.BLACK);
        // - -
        s = "[-" + ancho + ", " + alto + "]";
        myDrawString(s, 2, this.IMAGE_SIZE - 2, font, Color.BLACK);
        // + -
        s = "[-" + ancho + ", -" + alto + "]";
        myDrawString(s, this.IMAGE_SIZE - 65, this.IMAGE_SIZE - 2, font, Color.BLACK);
    }

    public void update(Observable arg0, Object arg1) {

        if (arg0 == grPrimitives) {

            paintWords();
            paintCoordenateAxes();

            shapeList = grPrimitives.getGrPrimitives();

            

//            double x = kalmanLps.getLPO(kalmanLps.BALL).getX();
//            double y = kalmanLps.getLPO(kalmanLps.BALL).getY();
//            double maxCoord = Math.max(Math.abs(x), Math.abs(y));
            //MAX_VISUAL_DIST = (int)(Math.ceil(maxCoord / 1000) * 1000);

            //paintBall();
            //paintBallBounds();

//            paintBlueNet();
//            paintBlueNetBounds();
            repaint();
        }
    }

    public void keyTyped(KeyEvent ke) {

    }

    public void keyPressed(KeyEvent ke) {

    }
}
