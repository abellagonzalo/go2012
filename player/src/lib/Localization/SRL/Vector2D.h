/*========================================================================
    This was taken from GVector.h originally, and split into 2 files:
    Vector2D.h & Vector3D.h

    Somchaya Liemhetcharat, 2009
  ========================================================================*/

/*========================================================================
    GVector.h : Simple vector class for 2D and 3D vectors
  ------------------------------------------------------------------------
    Copyright (C) 1999-2006  James R. Bruce
    School of Computer Science, Carnegie Mellon University
  ------------------------------------------------------------------------
    This software is distributed under the GNU General Public License,
    version 2.  If you do not have a copy of this licence, visit
    www.gnu.org, or write: Free Software Foundation, 59 Temple Place,
    Suite 330 Boston, MA 02111-1307 USA.  This program is distributed
    in the hope that it will be useful, but WITHOUT ANY WARRANTY,
    including MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  ========================================================================*/

#ifndef VECTOR2D_H_
#define VECTOR2D_H_

#include <math.h>
#include <boost/serialization/utility.hpp>

#include "num_util.h"

namespace GVector {


#define V2COMP(p) p.x,p.y

//=====================================================================//
//  Vector2D Class
//=====================================================================//

template <class num>
class vector2d{
public:
  num x,y;

  vector2d()
    {}
  vector2d(num nx,num ny)
    {x=nx; y=ny;}

  /// set the components of the vector
  void set(num nx,num ny)
    {x=nx; y=ny;}
  /// set the components of the vector to the same value
  void setAll(num nx)
    {x=y=nx;}
  /// set the components of the vector
  void set(vector2d<num> p)
    {x=p.x; y=p.y;}
  /// zero all components of the vector
  void zero()
    {x=y=0;}

  /// copy constructor
  vector2d<num> &operator=(vector2d<num> p)
    {set(p); return(*this);}

  /// element accessor
  num &operator[](int idx)
    {return(((num*)this)[idx]);}
  const num &operator[](int idx) const
    {return(((num*)this)[idx]);}

  /// calculate Euclidean length
  num length() const ;
  /// calculate squared Euclidean length (faster than length())
  num sqlength() const ;
  /// calculate the clockwise angle from <1,0>
  num angle() const
    {return(atan2(y,x));}
  /// make a unit vector at given angle
  void heading(num angle)
    {x=cos(angle); y=sin(angle);}

  /// return a unit length vector in the same direction
  vector2d<num> norm() const ;
  /// return a length 'len' vector in the same direction
  vector2d<num> norm(const num len) const ;
  /// normalize to unit length in place
  void normalize();
  /// bound vector to a maximum length
  vector2d<num> bound(const num max_length) const ;
  /// return if vector has any length at all
  bool nonzero() const
    {return(x!=0 || y!=0);}

  /// return dot product of vector with p
  num dot(const vector2d<num> p) const ;
  /// return dot product of vector with p, equivalent to (this->perp()).dot(p)
  num perpdot(const vector2d<num> p) const ;
  /// return z component of 3D cross product on 2D vectors.  right handed.
  num cross(const vector2d<num> p) const ;

  /// return the perpendicular of a vector (i.e. rotated 90 deg counter-clockwise)
  vector2d<num> perp() const
    {return(vector2d(-y, x));}

  /// add a vector to the current element values
  vector2d<num> &operator+=(const vector2d<num> p);
  /// subtract a vector from the current element values
  vector2d<num> &operator-=(const vector2d<num> p);
  /// multiply (elementwise) a vector with the current element values
  vector2d<num> &operator*=(const vector2d<num> p);
  /// divide (elementwise) a vector with the current element values
  vector2d<num> &operator/=(const vector2d<num> p);

  /// return vector sum of this vector and p
  vector2d<num> operator+(const vector2d<num> p) const;
  /// return vector difference of this vector and p
  vector2d<num> operator-(const vector2d<num> p) const;
  /// return elementwise product of this vector and p
  vector2d<num> operator*(const vector2d<num> p) const;
  /// return elementwise division of this vector by p
  vector2d<num> operator/(const vector2d<num> p) const;

  /// return this vector scaled by f
  vector2d<num> operator*(const num f) const;
  /// return this vector scaled by 1/f
  vector2d<num> operator/(const num f) const;
  /// scale this vector by f
  vector2d<num> &operator*=(num f);
  /// scale this vector by 1/f
  vector2d<num> &operator/=(num f);

  /// negate vector (reflect through origin) <x,y> -> <-x,-y>
  vector2d<num> operator-() const;

  bool operator==(const vector2d<num> p) const;
  bool operator!=(const vector2d<num> p) const;
  bool operator< (const vector2d<num> p) const;
  bool operator> (const vector2d<num> p) const;
  bool operator<=(const vector2d<num> p) const;
  bool operator>=(const vector2d<num> p) const;

  /// return vector rotated by angle a
  vector2d<num> rotate(const float a) const ;

  // vector2d<num> project(const vector2d<num> p) const;
  vector2d<num> project_in(const vector2d<num> p) const ;
  vector2d<num> project_out(const vector2d<num> p) const ;

  /// return true if both elements are finite, otherwise return false
  bool finite() const
    {return(::finite(x) && ::finite(y));}

  /// set the vector to the minimum of its components and p's components
  void take_min(const vector2d<num> p);
  /// set the vector to the maximum of its components and p's components
  void take_max(const vector2d<num> p);

private:
  /**
   * Allow access for Boost serialization
   */
  friend class boost::serialization::access;

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version) {
    ar & x;
    ar & y;
  }
};

template <class num>
num vector2d<num>::length() const
{
  return(sqrt(x*x + y*y));
}

template <class num>
num vector2d<num>::sqlength() const
{
  return(x*x + y*y);
}

template <class num>
vector2d<num> vector2d<num>::norm() const
{
  vector2d<num> p;
  num l;

  l = sqrt(x*x + y*y);
  p.x = x / l;
  p.y = y / l;

  return(p);
}

template <class num>
vector2d<num> vector2d<num>::norm(const num len) const
{
  vector2d<num> p;
  num f;

  f = len / sqrt(x*x + y*y);
  p.x = x * f;
  p.y = y * f;

  return(p);
}

template <class num>
void vector2d<num>::normalize()
{
  num l;

  l = sqrt(x*x + y*y);
  x /= l;
  y /= l;
}

template <class num>
vector2d<num> vector2d<num>::bound(const num max_length) const
{
  vector2d<num> p;
  num lsq,f;

  lsq = x*x + y*y;

  if(lsq < sq(max_length)){
    p.set(x,y);
  }else{
    f = max_length / sqrt(lsq);
    p.set(f*x,f*y);
  }

  return(p);
}

template <class num>
num vector2d<num>::dot(const vector2d<num> p) const
{
  return(x*p.x + y*p.y);
}

template <class num>
num vector2d<num>::perpdot(const vector2d<num> p) const
// perp product, equivalent to (this->perp()).dot(p)
{
  return(x*p.y - y*p.x);
}

template <class num>
num dot(const vector2d<num> a,const vector2d<num> b)
{
  return(a.x*b.x + a.y*b.y);
}

template <class num>
num cosine(const vector2d<num> a,const vector2d<num> b)
// equivalent to dot(a.norm(),b.norm())
{
  num l;

  l = sqrt(a.x*a.x + a.y*a.y) * sqrt(b.x*b.x + b.y*b.y);

  return((a.x*b.x + a.y*b.y) / l);
}

template <class num>
num vector2d<num>::cross(const vector2d<num> p) const
{
  // right handed
  return(x*p.y - p.x*y);
}

// returns point rotated by <a> radians
template <class num>
vector2d<num> vector2d<num>::rotate(const float a) const
{
  vector2d<num> q;
  float s,c;

  s = sin(a);
  c = cos(a);

  q.x = c*x + -s*y;
  q.y = s*x + c*y;

  return(q);
}

/* Depricated: replace "p.project(basis)" with "basis.project_out(p)"
/// returns vector projected onto (p, p.perp()) basis.
/// equivalent to q = p*x + p.perp()*y;
template <class num>
vector2d<num> vector2d<num>::project(const vector2d<num> p) const
{
  vector2d<num> q;

  q.x = p.x*x - p.y*y;
  q.y = p.y*x + p.x*y;

  return(q);
}
*/

/// takes a vector p in outer coordinate space and returns one
/// projected onto basis given by this,this.perp()
template <class num>
vector2d<num> vector2d<num>::project_in(const vector2d<num> p) const
{
  vector2d<num> q;
  q.x = x*p.x + y*p.y; // q.x = this->dot(p);
  q.y = x*p.y - y*p.x; // q.y = this->perpdot(p);
  return(q);
}

/// takes a vector p in basis given by this,this.perp() and returns
/// one in the outer coordinate space
template <class num>
vector2d<num> vector2d<num>::project_out(const vector2d<num> p) const
{
  vector2d<num> q;
  q.x = x*p.x - y*p.y;
  q.y = y*p.x + x*p.y;
  return(q);
}

#define VECTOR2D_EQUAL_BINARY_OPERATOR(opr) \
  template <class num> \
  vector2d<num> &vector2d<num>::operator opr (const vector2d<num> p) \
  {                  \
    x = x opr p.x;   \
    y = y opr p.y;   \
    return(*this);   \
  }

VECTOR2D_EQUAL_BINARY_OPERATOR(+=)
VECTOR2D_EQUAL_BINARY_OPERATOR(-=)
VECTOR2D_EQUAL_BINARY_OPERATOR(*=)
VECTOR2D_EQUAL_BINARY_OPERATOR(/=)

#define VECTOR2D_BINARY_OPERATOR(opr) \
  template <class num> \
  vector2d<num> vector2d<num>::operator opr (const vector2d<num> p) const \
  {                  \
    vector2d<num> r; \
    r.x = x opr p.x; \
    r.y = y opr p.y; \
    return(r);       \
  }

VECTOR2D_BINARY_OPERATOR(+)
VECTOR2D_BINARY_OPERATOR(-)
VECTOR2D_BINARY_OPERATOR(*)
VECTOR2D_BINARY_OPERATOR(/)

#define VECTOR2D_SCALAR_OPERATOR(opr) \
  template <class num> \
  vector2d<num> vector2d<num>::operator opr (const num f) const \
  {                  \
    vector2d<num> r;  \
    r.x = x opr f;   \
    r.y = y opr f;   \
    return(r);       \
  }

VECTOR2D_SCALAR_OPERATOR(*)
VECTOR2D_SCALAR_OPERATOR(/)

#define VECTOR2D_EQUAL_SCALAR_OPERATOR(opr) \
  template <class num> \
  vector2d<num> &vector2d<num>::operator opr (num f) \
  {                \
    x = x opr f;   \
    y = y opr f;   \
    return(*this); \
  }

VECTOR2D_EQUAL_SCALAR_OPERATOR(*=)
VECTOR2D_EQUAL_SCALAR_OPERATOR(/=)

#define VECTOR2D_LOGIC_OPERATOR(opr,combine) \
  template <class num> \
  bool vector2d<num>::operator opr (const vector2d<num> p) const \
  {                            \
    return((x opr p.x) combine \
           (y opr p.y));       \
  }

VECTOR2D_LOGIC_OPERATOR(==,&&)
VECTOR2D_LOGIC_OPERATOR(!=,||)

VECTOR2D_LOGIC_OPERATOR(< ,&&)
VECTOR2D_LOGIC_OPERATOR(> ,&&)
VECTOR2D_LOGIC_OPERATOR(<=,&&)
VECTOR2D_LOGIC_OPERATOR(>=,&&)


template <class num>
vector2d<num> vector2d<num>::operator-() const
{
  vector2d<num> r;
  r.x = -x;
  r.y = -y;
  return(r);
}

template <class num1,class num2>
vector2d<num2> operator*(num1 f,const vector2d<num2> &a)
{
  vector2d<num2> r;

  r.x = f * a.x;
  r.y = f * a.y;

  return(r);
}

template <class num>
void vector2d<num>::take_min(const vector2d<num> p)
{
  if(p.x < x) x = p.x;
  if(p.y < y) y = p.y;
}

template <class num>
void vector2d<num>::take_max(const vector2d<num> p)
{
  if(p.x > x) x = p.x;
  if(p.y > y) y = p.y;
}

template <class num>
inline vector2d<num> abs(vector2d<num> a)
{
  a.x = ::fabs(a.x);
  a.y = ::fabs(a.y);

  return(a);
}

template <class num>
inline vector2d<num> min(vector2d<num> a,vector2d<num> b)
{
  vector2d<num> v;

  v.x = ::min(a.x,b.x);
  v.y = ::min(a.y,b.y);

  return(v);
}

template <class num>
inline vector2d<num> max(vector2d<num> a,vector2d<num> b)
{
  vector2d<num> v;

  v.x = ::max(a.x,b.x);
  v.y = ::max(a.y,b.y);

  return(v);
}

template <class num>
inline vector2d<num> bound(vector2d<num> v,num low,num high)
{
  v.x = ::bound(v.x,low,high);
  v.y = ::bound(v.y,low,high);

  return(v);
}

template <class num>
num dist(const vector2d<num> a,const vector2d<num> b)
{
  num dx,dy;

  dx = a.x - b.x;
  dy = a.y - b.y;

  return(sqrt(dx*dx + dy*dy));
}

template <class num>
inline num distance(const vector2d<num> a,const vector2d<num> b)
{
  return(dist(a,b));
}

// returns square of distance between two points
template <class num>
num sqdist(const vector2d<num> a,const vector2d<num> b)
{
  num dx,dy;

  dx = a.x - b.x;
  dy = a.y - b.y;

  return(dx*dx + dy*dy);
}

template <class num>
inline num sqdistance(const vector2d<num> a,const vector2d<num> b)
{
  return(sqdist(a,b));
}

} // namespace GVector

typedef GVector::vector2d<float> vector2d;
typedef GVector::vector2d<float> Vector2D;

#endif /* VECTOR2D_H_ */
