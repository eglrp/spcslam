/*
A template-base coordinate transformation implementation
*/

#ifndef _SPCMAP_TRANSFORMATION_H_
#define _SPCMAP_TRANSFORMATION_H_


// Non-redundant transformation representation
// using translation and angle-axis

#define ZERO T(0.)

template<typename T>
class Transformation
{
public:
    //FIXME
    Transformation() : mrot(ZERO, ZERO, ZERO), mtrans(ZERO, ZERO, ZERO) {}
    
    Transformation(Vector3<T> trans, Vector3<T> rot) : mtrans(trans), mrot(rot) {}

    Transformation(const Vector3<T> & trans, const Quaternion<T> & qrot)
    : mtrans(trans), mrot(qrot.toRotationVector()) {}
    
    Transformation(const T * const data) : mtrans(data), mrot(data + 3) {}
    
    Transformation(T x, T y, T z, T rx, T ry, T rz)
    : mtrans(x, y, z), mrot(rx, ry, rz) {}
    
    Transformation(T x, T y, T z, T qx, T qy, T qz, T qw)
    : mtrans(x, y, z), mrot(Quaternion<T>(qx, qy, qz, qw).toRotationVector()) {}
    
    

    void toRotTrans(Matrix3<T> & R, Vector3<T> & t) const
    {
        t = mtrans;
        R = rotMat();
    }

    void toRotTransInv(Matrix3<T> & R, Vector3<T> & t) const
    {
        R = rotMat();
        R.transposeInPlace();
        t = -R*mtrans;
    }

    Transformation compose(const Transformation & transfo) const
    {
        Transformation res;
        Quaternion<T> q1(mrot), q2(transfo.mrot);
        res.mtrans = q1.rotate(transfo.mtrans) + mtrans;
        Quaternion<T> qres = q1 * q2;
        res.mrot = qres.toRotationVector();
        return res;
    }
    
    Transformation inverseCompose(const Transformation & transfo) const
    {
        Transformation res;
        Quaternion<T> q1(mrot), q2(transfo.mrot);
        Quaternion<T> q1inv = q1.inv();
        res.mtrans = q1inv.rotate(transfo.mtrans - mtrans);
        Quaternion<T> qres = q1inv * q2;
        res.mrot = qres.toRotationVector();
        return res;
    }

    const Vector3<T> & trans() const { return mtrans; }

    const Vector3<T> & rot() const { return mrot; }

    Vector3<T> & trans() { return mtrans; }

    Vector3<T> & rot() { return mrot; }

    Quaternion<T> rotQuat() const { return Quaternion<T>(mrot); } 
    
    Matrix3<T> rotMat() const { return rotationMatrix<T>(mrot); }

    T * rotData() { return mrot.data(); }
    T * transData() { return mtrans.data(); }

    friend ostream& operator << (ostream & os, const Transformation & transfo)
    {
        os << transfo.mtrans.transpose() << " # " << transfo.mrot.transpose();
        return os;
    }

    void transform(const vector<Vector3<T>> & src, vector<Vector3<T>> & dst) const
    {
        dst.resize(src.size());
        rotate(src, dst);
        for (auto & v : dst)
        {
            v += mtrans;
        }
    }
    
    void inverseTransform(const vector<Vector3<T>> & src, vector<Vector3<T>> & dst) const
    {
        dst.resize(src.size());
        for (unsigned int i = 0; i < src.size(); i++)
        {
            dst[i] = src[i] - mtrans;
        }
        inverseRotate(dst, dst);
    }

    void rotate(const vector<Vector3<T>> & src, vector<Vector3<T>> & dst) const
    {
        dst.resize(src.size());
        Matrix3<T> R = rotMat();
        for (unsigned int i = 0; i < src.size(); i++)
        {
            dst[i] = R * src[i];
        }
    }

    void inverseRotate(const vector<Vector3<T>> & src, vector<Vector3<T>> & dst) const
    {
        dst.resize(src.size());
        Matrix3<T> R = rotMat();
        R.transposeInPlace();
        for (unsigned int i = 0; i < src.size(); i++)
        {
            dst[i] = R * src[i];
        }
    }
    
    array<T, 6> toArray()
    {
        array<T, 6> res;
        copy(transData(), transData() + 3, res.data());
        copy(rotData(), rotData() + 3, res.data() + 3);
        return res;
    }
    
private:
    Vector3<T> mrot;
    Vector3<T> mtrans;

};

#endif
