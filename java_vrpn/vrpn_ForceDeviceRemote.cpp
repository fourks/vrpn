
#include "stdio.h"
#include "jni.h"
#include "vrpn_ForceDevice.h"

#include "java_vrpn.h"
#include "vrpn_ForceDeviceRemote.h"


JavaVM* jvm = NULL;
jclass jclass_vrpn_ForceDeviceRemote = NULL;
jfieldID jfid_vrpn_ForceDeviceRemote_native_force_device = NULL;


//////////////////////////
//  DLL functions


// This is called when the Java Virtual Machine loads this library
//  and sets some global references that are used elsewhere.
JNIEXPORT jint JNICALL JNI_OnLoad( JavaVM* jvm, void* reserved )
{
  //////////////
  // set the jvm reference
  ::jvm = jvm;

  ///////////////
  // get the JNIEnv pointer
  JNIEnv* env;
  if( jvm->GetEnv( (void**) &env, JAVA_VRPN_JNI_VERSION ) != JNI_OK )
  {
    printf( "Error loading vrpn ForceDeviceRemote native library.\n" );
    return JNI_ERR;
  }
  
  ///////////////
  // get the jclass reference to vrpn.ForceDeviceRemote
  jclass cls = env->FindClass( "vrpn/ForceDeviceRemote" );
  if( cls == NULL )
  {
    printf( "Error loading vrpn ForceDeviceRemote native library "
            "while trying to find class vrpn.ForceDeviceRemote.\n" );
    return JNI_ERR;
  }

  // make a global reference so the class can be referenced later.
  // this must be a _WEAK_ global reference, so the class will be
  // garbage collected, and so the JNI_OnUnLoad handler will be called
  jclass_vrpn_ForceDeviceRemote = (jclass) env->NewWeakGlobalRef( cls );
  if( jclass_vrpn_ForceDeviceRemote == NULL )
  {
    printf( "Error loading vrpn ForceDeviceRemote native library "
            "while setting up class vrpn.ForceDeviceRemote.\n" );
    return JNI_ERR;
  }

  ////////////////
  // get a jfid field id reference to the "native_force_device" 
  // field of class vrpn.ForceDeviceRemote.
  // field ids do not have to be made into global references.
  jfid_vrpn_ForceDeviceRemote_native_force_device 
    = env->GetFieldID( jclass_vrpn_ForceDeviceRemote, "native_force_device", "I" );
  if( jfid_vrpn_ForceDeviceRemote_native_force_device == NULL )
  {
    printf( "Error loading vrpn ForceDeviceRemote native library "
            "while looking into class vrpn.ForceDeviceRemote.\n" );
    return JNI_ERR;
  }
  
  return JAVA_VRPN_JNI_VERSION;
} // end JNI_OnLoad



JNIEXPORT void JNICALL JNI_OnUnload( JavaVM* jvm, void* reserved )
{
  // get the JNIEnv pointer
  JNIEnv* env;
  if( jvm->GetEnv( (void**) &env, JAVA_VRPN_JNI_VERSION ) != JNI_OK )
  {
    printf( "Error unloading vrpn ForceDeviceRemote native library.\n" );
    return;
  }

  // delete the global reference to the vrpn.ForceDeviceRemote class
  env->DeleteWeakGlobalRef( jclass_vrpn_ForceDeviceRemote );
}

// end DLL functions
/////////////////////////////


/////////////////////////////
// java_vrpn utility funcitons

void handle_force_change( void* userdata, const vrpn_FORCECB info )
{
  if( jvm == NULL )
  {
    printf( "Error in handle_force_change:  uninitialized jvm.\n" );
    return;
  }

  /*
  printf( "force change (C):  time:  %d.%d;\n"
          "\tforce: %f %f %f;\n",
          info.msg_time.tv_sec, info.msg_time.tv_usec,
          info.force[0], info.force[1], info.force[2] );
  */
  
  JNIEnv* env;
  jvm->AttachCurrentThread( (void**) &env, NULL );
  
  jobject jobj = (jobject) userdata;
  jclass jcls = env->GetObjectClass( jobj );
  jmethodID jmid = env->GetMethodID( jcls, "handleForceChange", "(JJDDD)V" );
  if( jmid == NULL )
  {
    printf( "Warning:  vrpn_ForceDeviceRemote library was unable to find the "
            "Java method \'handleForceChange\'.  This may indicate a version mismatch.\n" );
    return;
  }
  env->CallVoidMethod( jobj, jmid, (jlong) info.msg_time.tv_sec, (jlong) info.msg_time.tv_usec,
                       (jdouble) info.force[0], (jdouble) info.force[1], (jdouble) info.force[2] );

} // end handle_force_change


void handle_scp_change( void* userdata, const vrpn_FORCESCPCB info )
{
  if( jvm == NULL )
  {
    printf( "Error in handle_scp_change:  uninitialized jvm.\n" );
    return;
  }

  /*
  printf( "scp change (C):  time:  %d.%d;\n"
          "\pos: %f %f %f;\n\tquat:  %f %f %f\n",
          info.msg_time.tv_sec, info.msg_time.tv_usec,
          info.pos[0], info.pos[1], info.pos[2],
          info.quat[0], info.quat[1], info.quat[2], info.quat[3] );
  */
  
  JNIEnv* env;
  jvm->AttachCurrentThread( (void**) &env, NULL );
  
  jobject jobj = (jobject) userdata;
  jclass jcls = env->GetObjectClass( jobj );
  jmethodID jmid = env->GetMethodID( jcls, "handleSCPChange", "(JJDDDDDDD)V" );
  if( jmid == NULL )
  {
    printf( "Warning:  vrpn_ForceDeviceRemote library was unable to find the "
            "Java method \'handleSCPChange\'.  This may indicate a version mismatch.\n" );
    return;
  }
  env->CallVoidMethod( jobj, jmid, (jlong) info.msg_time.tv_sec, (jlong) info.msg_time.tv_usec,
                       (jdouble) info.pos[0], (jdouble) info.pos[1], (jdouble) info.pos[2],
                       (jdouble) info.quat[0], (jdouble) info.quat[1], (jdouble) info.quat[2], 
                       (jdouble) info.quat[3] );

} // end handle_scp_change


void handle_force_error( void* userdata, const vrpn_FORCEERRORCB info )
{
  if( jvm == NULL )
  {
    printf( "Error in handle_force_error:  uninitialized jvm.\n" );
    return;
  }

  /*
  printf( "force error (C):  time:  %d.%d;  force: %d;\n",
          info.msg_time.tv_sec, info.msg_time.tv_usec,
          info.error_code );
  */
  
  JNIEnv* env;
  jvm->AttachCurrentThread( (void**) &env, NULL );
  
  jobject jobj = (jobject) userdata;
  jclass jcls = env->GetObjectClass( jobj );
  jmethodID jmid = env->GetMethodID( jcls, "handleForceError", "(JJDDI)V" );
  if( jmid == NULL )
  {
    printf( "Warning:  vrpn_ForceDeviceRemote library was unable to find the "
            "Java method \'handleForceError\'.  This may indicate a version mismatch.\n" );
    return;
  }
  env->CallVoidMethod( jobj, jmid, (jlong) info.msg_time.tv_sec, (jlong) info.msg_time.tv_usec,
                       (jint) info.error_code );

} // end handle_force_error]


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    init
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL 
Java_vrpn_ForceDeviceRemote_init( JNIEnv* env, jobject jobj, jstring jname )
{
  printf( "in Java_ForceDeviceRemote_init(...)\n" );

  // look up where to store the force device pointer
  jclass jcls = env->GetObjectClass( jobj );
  jfieldID jfid = env->GetFieldID( jcls, "native_force_device", "I" );
  if( jfid == NULL )
  {
    printf( "Error in native method \"init\":  unable to ID native force device field.\n" );
    return false;
  }

  // make a global reference to the Java ForceDeviceRemote
  // object, so that it can be referenced in the functions
  // handle_*_change(...)
  jobj = env->NewGlobalRef( jobj );

  // create the force device
  const char* name = env->GetStringUTFChars( jname, NULL );
  vrpn_ForceDevice_Remote* f = new vrpn_ForceDevice_Remote( name );
  f->register_force_change_handler( jobj, handle_force_change );
  f->register_scp_change_handler( jobj, handle_scp_change );
  f->register_error_handler( jobj, handle_force_error );
  env->ReleaseStringUTFChars( jname, name );
  
  // now stash 'f' in the jobj's 'native_force_device' field
  jint jf = (jint) f;
  env->SetIntField( jobj, jfid, jf );
  
  return true;
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    mainloop
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_vrpn_ForceDeviceRemote_mainloop( JNIEnv* env, jobject jobj )
{
  // look up the force device pointer
  jclass jcls = env->GetObjectClass( jobj );
  jfieldID jfid = env->GetFieldID( jcls, "native_force_device", "I" );
  if( jfid == NULL )
  {
    printf( "Error in native method \"mainloop\":  unable to ID native force device field.\n" );
    return;
  }
  vrpn_ForceDevice_Remote* f = (vrpn_ForceDevice_Remote*) env->GetIntField( jobj, jfid );
  if( f <= 0 )  // this force device is uninitialized or has been shut down already
    return;

  // now call mainloop
  f->mainloop( );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    shutdownForceDevice
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_vrpn_ForceDeviceRemote_shutdownForceDevice( JNIEnv* env, jobject jobj )
{
  // look up where to store the force device pointer
  jclass jcls = env->GetObjectClass( jobj );
  jfieldID jfid = env->GetFieldID( jcls, "native_force_device", "I" );
  if( jfid == NULL )
  {
    printf( "Error in native method \"shutdownForceDevice\":  unable to ID native force device field.\n" );
    return;
  }

  // get the force deivce pointer
  vrpn_ForceDevice_Remote* f = (vrpn_ForceDevice_Remote*) env->GetIntField( jobj, jfid );
  
  // unregister a handler and destroy the force device
  if( f > 0 )
  {
    f->unregister_force_change_handler( jobj, handle_force_change );
    f->unregister_scp_change_handler( jobj, handle_scp_change );
    f->unregister_error_handler( jobj, handle_force_error );
    delete f;
  }

  // set the force device pointer to -1
  env->SetIntField( jobj, jfid, -1 );

  // delete global reference to object (that was created in init)
  env->DeleteGlobalRef( jobj );

}


// end java_vrpn utility functions
////////////////////////////////////


////////////////////////////////////
// native functions from java class ForceDeviceRemote

/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    sendForceField
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL 
Java_vrpn_ForceDeviceRemote_sendForceField__( JNIEnv* env, jobject jobj )
{
  // look up the force device pointer
  jclass jcls = env->GetObjectClass( jobj );
  jfieldID jfid = env->GetFieldID( jcls, "native_force_device", "I" );
  if( jfid == NULL )
    return false;
  vrpn_ForceDevice_Remote* f = (vrpn_ForceDevice_Remote*) env->GetIntField( jobj, jfid );
  if( f <= 0 )
    return false;
  
  f->sendForceField( );
  return true;

}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    stopForceField
 * Signature: ()V
 */
JNIEXPORT jboolean JNICALL 
Java_vrpn_ForceDeviceRemote_stopForceField( JNIEnv* env, jobject jobj )
{
  // look up the force device pointer
  jclass jcls = env->GetObjectClass( jobj );
  jfieldID jfid = env->GetFieldID( jcls, "native_force_device", "I" );
  if( jfid == NULL )
    return false;
  vrpn_ForceDevice_Remote* f = (vrpn_ForceDevice_Remote*) env->GetIntField( jobj, jfid );
  if( f <= 0 )
    return false;
  
  f->stopForceField( );
  return true;

}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    sendForceField
 * Signature: ([F[F[[FF)V
 */
JNIEXPORT jboolean JNICALL 
Java_vrpn_ForceDeviceRemote_sendForceField___3F_3F_3_3FF( JNIEnv* env, jobject jobj, 
                                                         jfloatArray jorigin, 
                                                         jfloatArray jforce, 
                                                         jobjectArray jjacobian, 
                                                         jfloat jradius )
{
  // look up the force device pointer
  jclass jcls = env->GetObjectClass( jobj );
  jfieldID jfid = env->GetFieldID( jcls, "native_force_device", "I" );
  if( jfid == NULL )
    return false;
  vrpn_ForceDevice_Remote* f = (vrpn_ForceDevice_Remote*) env->GetIntField( jobj, jfid );
  if( f <= 0 )
    return false;

  // get the arguments
  jfloat origin[3];
  jfloat force[3];
  jfloat jacobian_row0[3];
  jfloat jacobian_row1[3];
  jfloat jacobian_row2[3];
  jfloatArray jacobian_row;

  if( env->GetArrayLength( jorigin ) != 3 )
    return false;
  env->GetFloatArrayRegion( jorigin, 0, 3, origin);

  if( env->GetArrayLength( jforce ) != 3 )
    return false;
  env->GetFloatArrayRegion( jforce, 0, 3, force );
  if( force == NULL )
    return false;
  
  if( env->GetArrayLength( jjacobian ) != 3 )
    return false;
  jacobian_row = (jfloatArray) env->GetObjectArrayElement( jjacobian, 0 );
  if( jacobian_row == NULL )
    return false;
  if( env->GetArrayLength( jacobian_row ) != 3 )
    return false;
  env->GetFloatArrayRegion( jacobian_row, 0, 3, jacobian_row0 );

  jacobian_row = (jfloatArray) env->GetObjectArrayElement( jjacobian, 1 );
  if( jacobian_row == NULL )
    return false;
  if( env->GetArrayLength( jacobian_row ) != 3 )
    return false;
  env->GetFloatArrayRegion( jacobian_row, 0, 3, jacobian_row1 );

  jacobian_row = (jfloatArray) env->GetObjectArrayElement( jjacobian, 2 );
  if( jacobian_row == NULL )
    return false;
  if( env->GetArrayLength( jacobian_row ) != 3 )
    return false;
  env->GetFloatArrayRegion( jacobian_row, 0, 3, jacobian_row2 );

  
  float jacobian[3][3];
  jacobian[0][0] = jacobian_row0[0];
  jacobian[0][1] = jacobian_row0[1];
  jacobian[0][2] = jacobian_row0[2];
  jacobian[1][0] = jacobian_row1[0];
  jacobian[1][1] = jacobian_row1[1];
  jacobian[1][2] = jacobian_row1[2];
  jacobian[2][0] = jacobian_row1[0];
  jacobian[2][1] = jacobian_row1[1];
  jacobian[2][2] = jacobian_row1[2];


  // now call the function
  f->sendForceField( origin, force, jacobian, jradius );
  return true;

}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    enableConstraint
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_enableConstraint
  (JNIEnv *, jobject, jint)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_enableConstraint is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setConstraintMode
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setConstraintMode
  (JNIEnv *, jobject, jint)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintMode is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setConstraintPoint
 * Signature: ([F)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setConstraintPoint
  (JNIEnv *, jobject, jfloatArray)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintPoint is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setConstraintLinePoint
 * Signature: ([F)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setConstraintLinePoint
  (JNIEnv *, jobject, jfloatArray)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintLinePoint is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setConstraintLineDirection
 * Signature: ([F)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setConstraintLineDirection
  (JNIEnv *, jobject, jfloatArray)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintLineDirection is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setConstraintPlaneNormal
 * Signature: ([F)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setConstraintPlaneNormal
  (JNIEnv *, jobject, jfloatArray)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintPlaneNormal is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setConstraintPlanePoint
 * Signature: ([F)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setConstraintPlanePoint
  (JNIEnv *, jobject, jfloatArray)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintPlanePoint is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setConstraintKSpring
 * Signature: (F)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setConstraintKSpring
  (JNIEnv *, jobject, jfloat)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintKSpring is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setTriangle
 * Signature: (IIIIIII)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setTriangle
  (JNIEnv *, jobject, jint, jint, jint, jint, jint, jint, jint)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setTriangle is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    useHcollide
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_useHcollide
  (JNIEnv *, jobject)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_useHcollide is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    useGhost
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_useGhost
  (JNIEnv *, jobject)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_useGhost is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setTrimeshTransform
 * Signature: ([F)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setTrimeshTransform
  (JNIEnv *, jobject, jfloatArray)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setTrimeshTransform is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    updateTrimeshChanges
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_updateTrimeshChanges
  (JNIEnv *, jobject)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_updateTrimeshChanges is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    clearTrimesh
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_clearTrimesh
  (JNIEnv *, jobject)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_clearTrimesh is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    sendSurface
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_sendSurface
  (JNIEnv *, jobject)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_sendSurface is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    startSurface
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_startSurface
  (JNIEnv *, jobject)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_startSurface is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    stopSurface
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_stopSurface
  (JNIEnv *, jobject)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_stopSurface is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setVertex
 * Signature: (IFFF)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setVertex
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setVertex is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    setNormal
 * Signature: (IFFF)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_setNormal
  (JNIEnv *, jobject, jint, jfloat, jfloat, jfloat)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setNormal is not implemented.\n" );
}


/*
 * Class:     vrpn_ForceDeviceRemote
 * Method:    removeTriangle
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_vrpn_ForceDeviceRemote_removeTriangle
  (JNIEnv *, jobject, jint)
{
  printf( "Function Java_vrpn_ForceDeviceRemote_setConstraintMode is not implemented.\n" );
}


// end native functions for java class ForceDeviceRemote
///////////////////////////////////////////


