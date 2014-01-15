/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 2.0.10
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package com.badlogic.gdx.physics.bullet.collision;

import com.badlogic.gdx.physics.bullet.BulletBase;
import com.badlogic.gdx.physics.bullet.linearmath.*;
import com.badlogic.gdx.math.Vector3;
import com.badlogic.gdx.math.Quaternion;
import com.badlogic.gdx.math.Matrix3;
import com.badlogic.gdx.math.Matrix4;

public class btPolyhedralConvexAabbCachingShape extends btPolyhedralConvexShape {
	private long swigCPtr;
	
	protected btPolyhedralConvexAabbCachingShape(final String className, long cPtr, boolean cMemoryOwn) {
		super(className, CollisionJNI.btPolyhedralConvexAabbCachingShape_SWIGUpcast(cPtr), cMemoryOwn);
		swigCPtr = cPtr;
	}
	
	/** Construct a new btPolyhedralConvexAabbCachingShape, normally you should not need this constructor it's intended for low-level usage. */
	public btPolyhedralConvexAabbCachingShape(long cPtr, boolean cMemoryOwn) {
		this("btPolyhedralConvexAabbCachingShape", cPtr, cMemoryOwn);
		construct();
	}
	
	@Override
	protected void reset(long cPtr, boolean cMemoryOwn) {
		if (!destroyed)
			destroy();
		super.reset(CollisionJNI.btPolyhedralConvexAabbCachingShape_SWIGUpcast(swigCPtr = cPtr), cMemoryOwn);
	}
	
	public static long getCPtr(btPolyhedralConvexAabbCachingShape obj) {
		return (obj == null) ? 0 : obj.swigCPtr;
	}

	@Override
	protected void finalize() throws Throwable {
		if (!destroyed)
			destroy();
		super.finalize();
	}

  @Override protected synchronized void delete() {
		if (swigCPtr != 0) {
			if (swigCMemOwn) {
				swigCMemOwn = false;
				CollisionJNI.delete_btPolyhedralConvexAabbCachingShape(swigCPtr);
			}
			swigCPtr = 0;
		}
		super.delete();
	}

  public void getNonvirtualAabb(Matrix4 trans, Vector3 aabbMin, Vector3 aabbMax, float margin) {
    CollisionJNI.btPolyhedralConvexAabbCachingShape_getNonvirtualAabb(swigCPtr, this, trans, aabbMin, aabbMax, margin);
  }

  public void recalcLocalAabb() {
    CollisionJNI.btPolyhedralConvexAabbCachingShape_recalcLocalAabb(swigCPtr, this);
  }

}
