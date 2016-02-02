//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsIReflectable.h"
#include "BsMatrix4.h"
#include "BsVector3.h"
#include "BsVector2.h"
#include "BsVector2I.h"
#include "BsAABox.h"
#include "BsQuaternion.h"
#include "BsRay.h"
#include "BsCoreObject.h"
#include "BsConvexVolume.h"

namespace BansheeEngine 
{
	/** @cond INTERNAL */
	/** @addtogroup Renderer-Engine
	 *  @{
	 */

	/**	Projection type to use by the camera. */
    enum ProjectionType
    {
		PT_ORTHOGRAPHIC, /**< Projection type where object size remains constant and parallel lines remain parallel. */
		PT_PERSPECTIVE /**< Projection type that emulates human vision. Objects farther away appear smaller. */
    };

	/**	Clip planes that form the camera frustum (visible area). */
    enum FrustumPlane
    {
        FRUSTUM_PLANE_NEAR = 0,
        FRUSTUM_PLANE_FAR = 1,
        FRUSTUM_PLANE_LEFT = 2,
        FRUSTUM_PLANE_RIGHT = 3,
        FRUSTUM_PLANE_TOP = 4,
        FRUSTUM_PLANE_BOTTOM = 5
    };

	/**	Flags that describe a camera. */
	enum class CameraFlags
	{
		/** 
		 * This flag is a signal to the renderer that his camera will only render overlays and doesn't require depth   
		 * buffer or multi-sampled render targets. This can improve performance and memory usage. 
		 */
		Overlay = 1
	};

	/** @} */
	/** @addtogroup Implementation
	 *  @{
	 */

	/**
	 * Camera determines how is world geometry projected onto a 2D surface. You may position and orient it in space, set
	 * options like aspect ratio and field or view and it outputs view and projection matrices required for rendering.
	 */
	class BS_EXPORT CameraBase
    {
    public:
		virtual ~CameraBase() { }

		/**
		 * Sets the camera horizontal field of view. This determines how wide the camera viewing angle is along the
		 * horizontal axis. Vertical FOV is calculated from the horizontal FOV and the aspect ratio.
		 */
        virtual void setHorzFOV(const Radian& fovy);

		/**	Retrieves the camera horizontal field of view. */
        virtual const Radian& getHorzFOV() const;

		/**
		 * Sets the distance from the frustum to the near clipping plane. Anything closer than the near clipping plane will
		 * not be rendered. Decreasing this value decreases depth buffer precision.
		 */
        virtual void setNearClipDistance(float nearDist);

		/**
		 * Retrieves the distance from the frustum to the near clipping plane. Anything closer than the near clipping plane
		 * will not be rendered. Decreasing this value decreases depth buffer precision.
		 */
        virtual float getNearClipDistance() const;

		/**
		 * Sets the distance from the frustum to the far clipping plane. Anything farther than the far clipping plane will
		 * not be rendered. Increasing this value decreases depth buffer precision.
		 */
        virtual void setFarClipDistance(float farDist);

		/**
		 * Retrieves the distance from the frustum to the far clipping plane. Anything farther than the far clipping plane
		 * will not be rendered. Increasing this value decreases depth buffer precision.
		 */
        virtual float getFarClipDistance() const;

		/**	Sets the current viewport aspect ratio (width / height). */
        virtual void setAspectRatio(float ratio);

		/**	Returns current viewport aspect ratio (width / height). */
        virtual float getAspectRatio() const;

		/**	Sets camera world space position. */
		virtual void setPosition(const Vector3& position);

		/**	Retrieves camera world space position. */
		virtual Vector3 getPosition() const { return mPosition; }

		/**	Sets should the camera be rendered to or not. */
		void setIsActive(bool active) { mIsActive = active; _markCoreDirty(); }
		
		/**	Gets whether the camera be rendered to or not. */
		bool getIsActive() const { return mIsActive; }

		/**
		 * Gets the Z (forward) axis of the object, in world space.
		 *
		 * @return	Forward axis of the object.
		 */
		Vector3 getForward() const { return getRotation().rotate(-Vector3::UNIT_Z); }

		/**	Sets camera world space rotation. */
		virtual void setRotation(const Quaternion& rotation);

		/**	Retrieves camera world space rotation. */
		virtual Quaternion getRotation() const { return mRotation; }

		/** Manually set the extents of the frustum that will be used when calculating the projection matrix. This will
		 * prevents extents for being automatically calculated from aspect and near plane so it is up to the caller to keep
		 * these values accurate.
		 *
		 * @param[in] left		The position where the left clip plane intersect the near clip plane, in view space.
		 * @param[in] right		The position where the right clip plane intersect the near clip plane, in view space.
		 * @param[in] top		The position where the top clip plane intersect the near clip plane, in view space.
		 * @param[in] bottom	The position where the bottom clip plane intersect the near clip plane, in view space.
		*/
		virtual void setFrustumExtents(float left, float right, float top, float bottom);

		/** 
		 * Resets frustum extents so they are automatically derived from other values. This is only relevant if you have
		 * previously set custom extents.
		 */
		virtual void resetFrustumExtents(); 

		/** Returns the extents of the frustum in view space at the near plane. */
		virtual void getFrustumExtents(float& outleft, float& outright, float& outtop, float& outbottom) const;

		/** 
		 * Returns the standard projection matrix that determines how are 3D points projected to two dimensions. The layout
		 * of this matrix depends on currently used render system.
		 *
		 * @note	
		 * You should use this matrix when sending the matrix to the render system to make sure everything works 
		 * consistently when other render systems are used.
		 */
        virtual const Matrix4& getProjectionMatrixRS() const;

		/** 
		 * Returns the inverse of the render-system specific projection matrix.
		 *
		 * @see		getProjectionMatrixRS
		 */
        virtual const Matrix4& getProjectionMatrixRSInv() const;

		/** 
		 * Returns the standard projection matrix that determines how are 3D points projected to two dimensions. Returned
		 * matrix is standard following right-hand rules and depth range of [-1, 1]. 
		 *
		 * @note	
		 * Different render systems will expect different projection matrix layouts, in which case use 
		 * getProjectionMatrixRS().
         */
        virtual const Matrix4& getProjectionMatrix() const;

		/** 
		 * Returns the inverse of the projection matrix.
		 *
		 * @see		getProjectionMatrix
		 */
        virtual const Matrix4& getProjectionMatrixInv() const;

		/** Gets the camera view matrix. Used for positioning/orienting the camera. */
        virtual const Matrix4& getViewMatrix() const;

		/** 
		 * Returns the inverse of the view matrix.
		 *
		 * @see		getViewMatrix
		 */
		virtual const Matrix4& getViewMatrixInv() const;

		/** 
		 * Sets whether the camera should use the custom view matrix. When this is enabled camera will no longer calculate
		 * its view matrix based on position/orientation and caller will be resonsible to keep the view matrix up to date.
         */
		virtual void setCustomViewMatrix(bool enable, const Matrix4& viewMatrix = Matrix4::IDENTITY);

		/** Returns true if a custom view matrix is used. */
		virtual bool isCustomViewMatrixEnabled() const { return mCustomViewMatrix; }
		
		/** 
		 * Sets whether the camera should use the custom projection matrix. When this is enabled camera will no longer
		 * calculate its projection matrix based on field of view, aspect and other parameters and caller will be resonsible
		 * to keep the projection matrix up to date.
         */
		virtual void setCustomProjectionMatrix(bool enable, const Matrix4& projectionMatrix = Matrix4::IDENTITY);

		/** Returns true if a custom projection matrix is used. */
		virtual bool isCustomProjectionMatrixEnabled() const { return mCustomProjMatrix; }

		/** Returns a convex volume representing the visible area of the camera, in local space. */
        virtual const ConvexVolume& getFrustum() const;

		/** Returns a convex volume representing the visible area of the camera, in world space. */
        virtual ConvexVolume getWorldFrustum() const;

		/**	Returns the bounding of the frustum. */
        const AABox& getBoundingBox() const;

		/**
		 * Sets the type of projection used by the camera. Projection type controls how is 3D geometry projected onto a 
		 * 2D plane.
		 */
        virtual void setProjectionType(ProjectionType pt);

		/**
		 * Returns the type of projection used by the camera. Projection type controls how is 3D geometry projected onto a
		 * 2D plane.
		 */
        virtual ProjectionType getProjectionType() const;

		/**
		 * Sets the orthographic window height, for use with orthographic rendering only. 
		 *
		 * @param[in]	w	Width of the window in world units.
		 * @param[in]	h	Height of the window in world units.
		 *
		 * @note	
		 * Calling this method will recalculate the aspect ratio, use setOrthoWindowHeight() or setOrthoWindowWidth() alone
		 * if you wish to preserve the aspect ratio but just fit one or other dimension to a particular size.
		 */
		virtual void setOrthoWindow(float w, float h);

		/**
		 * Sets the orthographic window height, for use with orthographic rendering only. 
		 *
		 * @param[in]	h	Height of the window in world units.
		 *
		 * @note	The width of the window will be calculated from the aspect ratio. 
		 */
		virtual void setOrthoWindowHeight(float h);

		/**
		 * Sets the orthographic window width, for use with orthographic rendering only. 
		 *
		 * @param[in]	w	Width of the window in world units.
		 *
		 * @note	The height of the window will be calculated from the aspect ratio. 
		 */
		virtual void setOrthoWindowWidth(float w);

		/** Gets the orthographic window width in world units, for use with orthographic rendering only. */
		virtual float getOrthoWindowHeight() const;

		/**
		 * Gets the orthographic window width in world units, for use with orthographic rendering only. 
		 *
		 * @note	This is calculated from the orthographic height and the aspect ratio.
		 */
		virtual float getOrthoWindowWidth() const;

		/**
		 * Gets a priority that determines in which orders the cameras are rendered. This only applies to cameras rendering
		 * to the same render target. 
		 */
		INT32 getPriority() const { return mPriority; }

		/**
		 * Sets a priority that determines in which orders the cameras are rendered. This only applies to cameras rendering
		 * to the same render target. 
		 *
		 * @param[in]	priority	The priority. Higher value means the camera will be rendered sooner.
		 */
		void setPriority(INT32 priority) { mPriority = priority; _markCoreDirty(); }

		/**	Retrieves layer bitfield that is used when determining which object should the camera render. */
		UINT64 getLayers() const { return mLayers; }

		/**	Sets layer bitfield that is used when determining which object should the camera render. */
		void setLayers(UINT64 layers) { mLayers = layers; _markCoreDirty(); }

		/**	Retrieves flags that define the camera. */
		CameraFlags getFlags() const { return (CameraFlags)mCameraFlags; }

		/**
		 * @brief	Sets flags that define the camera.
		 */
		void setFlags(const CameraFlags& flags) { mCameraFlags = (UINT32)flags; _markCoreDirty(); }

		/**
		 * Converts a point in world space to screen coordinates (in pixels corresponding to the render target attached to
		 * the camera).
		 */
		Vector2I worldToScreenPoint(const Vector3& worldPoint) const;

		/**	Converts a point in world space to normalized clip coordinates (in [0, 1] range). */
		Vector2 worldToClipPoint(const Vector3& worldPoint) const;

		/** Converts a point in world space to point relative to camera's coordinate system (view space). */
		Vector3 worldToViewPoint(const Vector3& worldPoint) const;

		/**
		 * Converts a point in screen space (pixels corresponding to render target attached to the camera) to a point in
		 * world space.
		 *
		 * @param[in]	screenPoint	Point to transform.
		 * @param[in]	depth		Depth to place the world point at. The depth is applied to the vector going from camera
		 *							origin to the point on the near plane.
		 */
		Vector3 screenToWorldPoint(const Vector2I& screenPoint, float depth = 0.5f) const;

		/**
		 * Converts a point in screen space (pixels corresponding to render target attached to the camera) to a point
		 * relative to camera's coordinate system (view space).
		 *
		 * @param[in]	screenPoint	Point to transform.
		 * @param[in]	depth		Depth to place the world point at. The depth is applied to the vector going from camera
		 *							origin to the point on the near plane.
		 */
		Vector3 screenToViewPoint(const Vector2I& screenPoint, float depth = 0.5f) const;

		/**
		 * Converts a point in screen space (pixels corresponding to render target attached to the camera) to normalized 
		 * clip coordinates (in [0, 1] range).
		 */
		Vector2 screenToClipPoint(const Vector2I& screenPoint) const;

		/** Converts a point relative to camera's coordinate system (view space) into a point in world space. */
		Vector3 viewToWorldPoint(const Vector3& viewPoint) const;

		/**
		 * Converts a point relative to camera's coordinate system (view space) into a point in screen space (pixels 
		 * corresponding to render target attached to the camera).
		 */
		Vector2I viewToScreenPoint(const Vector3& viewPoint) const;

		/**
		 * Converts a point relative to camera's coordinate system (view space) into normalized clip coordinates 
		 * (in [0, 1] range).
		 */
		Vector2 viewToClipPoint(const Vector3& viewPoint) const;

		/**
		 * Converts a point in normalized clip coordinates ([0, 1] range) to a point in world space.
		 *
		 * @param[in]	clipPoint	Point to transform.
		 * @param[in]	depth		Depth to place the world point at. The depth is applied to the vector going from camera
		 *							origin to the point on the near plane.
		 */
		Vector3 clipToWorldPoint(const Vector2& clipPoint, float depth = 0.5f) const;

		/**
		 * Converts a point in normalized clip coordinates ([0, 1] range) to a point relative to camera's coordinate system
		 * (view space).
		 *
		 * @param[in]	clipPoint	Point to transform.
		 * @param[in]	depth		Depth to place the world point at. The depth is applied to the vector going from camera
		 *							origin to the point on the near plane.
		 */
		Vector3 clipToViewPoint(const Vector2& clipPoint, float depth = 0.5f) const;

		/**
		 * Converts a point in normalized clip coordinates ([0, 1] range) to a point in screen space (pixels corresponding
		 * to render target attached to the camera).
		 */
		Vector2I clipToScreenPoint(const Vector2& clipPoint) const;

		/**
		 * Converts a point in screen space (pixels corresponding to render target attached to the camera) to a ray in world
		 * space originating at the selected point on the camera near plane.
		 */
		Ray screenPointToRay(const Vector2I& screenPoint) const;

		/**	Projects a point from view to clip space. */
		Vector3 projectPoint(const Vector3& point) const;

		/**	Un-projects a point in clip space to view space. */
		Vector3 unprojectPoint(const Vector3& point) const;

        static const float INFINITE_FAR_PLANE_ADJUST; /**< Small constant used to reduce far plane projection to avoid inaccuracies. */

	protected:
		CameraBase();

		/**	Calculate projection parameters that are used when constructing the projection matrix. */
		virtual void calcProjectionParameters(float& left, float& right, float& bottom, float& top) const;

		/**	Recalculate frustum if dirty. */
		virtual void updateFrustum() const;

		/**	Recalculate frustum planes if dirty. */
		virtual void updateFrustumPlanes() const;

		/**
		 * Update view matrix from parent position/orientation.
		 *
		 * @note	Does nothing when custom view matrix is set.
		 */
		virtual void updateView() const;

		/**	Checks if the frustum requires updating. */
		virtual bool isFrustumOutOfDate() const;

		/**	Notify camera that the frustum requires to be updated. */
		virtual void invalidateFrustum() const;

		/**	Returns a rectangle that defines the viewport position and size, in pixels. */
		virtual Rect2I getViewportRect() const = 0;

		/** @copydoc CoreObject::markCoreDirty */
		virtual void _markCoreDirty() { }

    protected:
		UINT64 mLayers; /**< Bitfield that can be used for filtering what objects the camera sees. */
		UINT32 mCameraFlags; /**< Flags that further determine type of camera. */

		Vector3 mPosition; /**< World space position. */
		Quaternion mRotation; /**< World space rotation. */
		bool mIsActive; /**< Is camera being rendered to. */

		ProjectionType mProjType; /**< Type of camera projection. */
		Radian mHorzFOV; /**< Horizontal field of view represents how wide is the camera angle. */
		float mFarDist; /**< Clip any objects further than this. Larger value decreases depth precision at smaller depths. */
		float mNearDist; /**< Clip any objects close than this. Smaller value decreases depth precision at larger depths. */
		float mAspect; /**< Width/height viewport ratio. */
		float mOrthoHeight; /**< Height in world units used for orthographic cameras. */
		INT32 mPriority; /**< Determines in what order will the camera be rendered. Higher priority means the camera will be rendered sooner. */

		bool mCustomViewMatrix; /**< Is custom view matrix set. */
		bool mCustomProjMatrix; /**< Is custom projection matrix set. */

		bool mFrustumExtentsManuallySet; /**< Are frustum extents manually set. */

		mutable Matrix4 mProjMatrixRS; /**< Cached render-system specific projection matrix. */
		mutable Matrix4 mProjMatrix; /**< Cached projection matrix that determines how are 3D points projected to a 2D viewport. */
		mutable Matrix4 mViewMatrix; /**< Cached view matrix that determines camera position/orientation. */
		mutable Matrix4 mProjMatrixRSInv;
		mutable Matrix4 mProjMatrixInv;
		mutable Matrix4 mViewMatrixInv;

		mutable ConvexVolume mFrustum; /**< Main clipping planes describing cameras visible area. */
		mutable bool mRecalcFrustum; /**< Should frustum be recalculated. */
		mutable bool mRecalcFrustumPlanes; /**< Should frustum planes be recalculated. */
		mutable bool mRecalcView; /**< Should view matrix be recalculated. */
		mutable float mLeft, mRight, mTop, mBottom; /**< Frustum extents. */
		mutable AABox mBoundingBox; /**< Frustum bounding box. */
     };

	/** @} */
	/** @addtogroup Renderer-Engine
	 *  @{
	 */

	/** @copydoc CameraBase */
	class BS_EXPORT CameraCore : public CoreObjectCore, public CameraBase
	{
	public:
		~CameraCore();

		/**	Returns the viewport used by the camera. */	
		SPtr<ViewportCore> getViewport() const { return mViewport; }

	protected:
		friend class Camera;

		CameraCore(SPtr<RenderTargetCore> target = nullptr,
			float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);

		CameraCore(const SPtr<ViewportCore>& viewport);

		/** @copydoc CoreObjectCore::initialize */
		void initialize() override;

		/** @copydoc CameraBase */
		virtual Rect2I getViewportRect() const override;

		/** @copydoc CoreObject::syncToCore */
		void syncToCore(const CoreSyncData& data) override;

		SPtr<ViewportCore> mViewport;
	};

	/** @copydoc CameraBase */
	class BS_EXPORT Camera : public IReflectable, public CoreObject, public CameraBase
    {
    public:
		/**	Returns the viewport used by the camera. */	
		ViewportPtr getViewport() const { return mViewport; }

		/**
		 * Determines whether this is the main application camera. Main camera controls the final render surface that is
		 * displayed to the user.
		 */	
		bool isMain() const { return mMain; }

		/**
		 * Marks or unmarks this camera as the main application camera. Main camera controls the final render surface that
		 * is displayed to the user.
		 */	
		void setMain(bool main) { mMain = main; }

		/** Retrieves an implementation of a camera handler usable only from the core thread. */
		SPtr<CameraCore> getCore() const;

		/**	Creates a new camera that renders to the specified portion of the provided render target. */
		static CameraPtr create(RenderTargetPtr target = nullptr,
			float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);

		/** @cond INTERNAL */

		/**	Returns the hash value that can be used to identify if the internal data needs an update. */
		UINT32 _getLastModifiedHash() const { return mLastUpdateHash; }

		/**	Sets the hash value that can be used to identify if the internal data needs an update. */
		void _setLastModifiedHash(UINT32 hash) { mLastUpdateHash = hash; }

		/** @endcond */

	protected:
		Camera(RenderTargetPtr target = nullptr,
			float left = 0.0f, float top = 0.0f, float width = 1.0f, float height = 1.0f);

		/** @copydoc CameraBase */
		virtual Rect2I getViewportRect() const override;

		/** @copydoc CoreObject::createCore */
		SPtr<CoreObjectCore> createCore() const override;

		/** @copydoc CoreObject::markCoreDirty */
		void _markCoreDirty() override;

		/** @copydoc CoreObject::syncToCore */
		CoreSyncData syncToCore(FrameAlloc* allocator) override;

		/** @copydoc CoreObject::getCoreDependencies */
		void getCoreDependencies(Vector<CoreObject*>& dependencies) override;

		/**	Creates a new camera without initializing it. */
		static CameraPtr createEmpty();

		ViewportPtr mViewport; /**< Viewport that describes 2D rendering surface. */
		bool mMain;
		UINT32 mLastUpdateHash;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class CameraRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
     };

	/** @} */
	/** @endcond */
}