#ifndef LF_SCENE_H
#define LF_SCENE_H

#include <string>
//#include <vector>

#include "optical_flow/CvUtil.h"
#include "pinholeCamera.h"

void loadPFM(std::vector<float>& output,
             int width, int height,
             std::string name,
             bool flip = false);
void loadPFM(std::vector<cv::Point2f>& output,
             int width, int height,
             std::string name,
             bool flip = false);
void loadPFM(std::vector<cv::Point3f>& output,
             int width, int height,
             std::string name,
             bool flip = false);
void loadPFM(std::vector<bool>& output,
             int width, int height,
             std::string name,
             bool flip = false);

void savePFM(std::vector<std::vector<float> >& input,
             int width, int height,
             std::string name,
             bool flip = false);
void savePFM(std::vector<bool>& input,
             int width, int height,
             std::string name,
             bool flip = false);
void savePFM(std::vector<float>& input,
             int width, int height,
             std::string name,
             bool flip = false);
void savePFM(std::vector<cv::Point2f>& input,
             int width, int height,
             std::string name,
             bool flip = false);
void savePFM(std::vector<cv::Point3f>& input,
             int width, int height,
             std::string name,
             bool flip = false);
void savePNG(std::vector<cv::Point3f>& input,
             int width, int height,
             std::string name,
             bool flip = false);

// backward warping of one view, bilinear interpolation
// convention: 0 coordinate is at the center
void bilinearInterpolation( int width, int height,
                            const std::vector<cv::Point3f>& inputImage,
                            cv::Point3f& outputColor,
                            const cv::Point2f& dest );

class InputCam {

public:

    InputCam( uint W, uint H, std::string outdir );
    ~InputCam();

    // Import camera image from PNG file
    bool importTexture( const char* imageName, bool flip = false );

    // Import camera parameters and load vbos
    // Read the parameters from Stanford images files
    bool importCamParametersStanford( double centerX, double centerY );

    // Import camera parameters and load vbos
    // Read the camera matrices from INI file (MVE format)
    bool importCamParameters( char *cameraName );

    // getters
    PinholeCamera getPinholeCamera() const;
    const std::vector<cv::Point3f>& getTextureRGB() const;

private:

    bool importMVEIFile( char *fileName, std::vector< std::vector<float> > &data, uint &W, uint &H );

    uint _W; uint _H;
    std::string _outdir;
    PinholeCamera _pinholeCamera;
    std::vector<cv::Point3f> _texture;
};

class LFScene {

public:

    LFScene(std::string outdir,
            std::string winTitle,
            std::string mveName,
            int windowW1, int windowW2, int windowH1, int windowH2,
            uint camWidth, uint camHeight,
            int sMin, int sMax, int sRmv,
            int tMin, int tMax, int tRmv);
    ~LFScene();

    // IMPORT_LF
    // ---------------------------------------------------------------------------------------------------------- //

    // load standford lightfield dataset (image and camera matrices)
    // camera matrices are obainted thank to openMVG calibration
    // MVE FORMAT (convert rows to rows and column)
    void importStanfordMVEViews( );

    // same as above (without target view)
    // custom camera configuration
    void importCustomMVEViews();

    // for each view import source images and camera parameters (blender datasets for example)
    // BLENDER FORMAT (rows only)
    void importViewsNoDepth( std::string cameraName, std::string imageName = 0 );

    // IBR_optical
    // ---------------------------------------------------------------------------------------------------------- //
    void computeVisibilityMask(std::vector<cv::Point2f>& flowLtoR, std::vector<cv::Point2f>& flowRtoL,
                               std::vector<bool>& visibilityMask);

    void computePerPixelCorresp(std::string imageName,
                                std::string flowAlg);

    // old function that computes 3D points from optical flow (LF flow), and compare with classic triangulation (minimization of reproj error)
    void computeFlowedLightfield();

    // for stanford dataset
    // source images -> optical flow
    void computePerPixelCorrespStarConfig(std::string flowAlg);

    // run optical flow on custom config
    // source images -> optical flow
    void computePerPixelCorrespCustomConfig(std::string flowAlg);

    // for stanford dataset
    // source images -> optical flow
    void computePerPixelCorrespBandConfig(std::string flowAlg);

    // compute flowed lightfield from optical flow for Stanford-like datasets (star config)
    // optical flow -> flowed LF
    void computeFlowedLFStarConfig();

    // compute flowed lightfield from optical flow for Stanford-like datasets (custom config)
    // optical flow -> flowed LF
    void computeFlowedLFCustomConfig();

    // to test the triangulated 3D point
    // print some info
    void testTriangulation(uint x, uint y);

    // new function that fits a linear 4D ray model from light flow samples
    // flowed LF -> estimated model (triangulation)
    void curveFitting();

    // fits a linear color model from light flow color samples
    void curveFittingColor();

    // import target camera parameters
    void loadTargetView(cv::Mat &targetK, cv::Mat &targetR, cv::Point3f &targetC, std::string cameraName);
    void loadTargetView(cv::Mat &targetK, cv::Mat &targetR, cv::Point3f &targetC);

    // compute the bayesian information criterion (BIC) for model selection
    void bic();

    // render nove view by interpolating the light flow
    void renderLightFlow();
    void renderLightFlowLambertianModel();
    void renderLightFlowVideo();
    void renderLightFlowLambertianVideo();

private:

    std::string _outdir;
    std::string _windowTitle;
    std::string _mveName;
    uint _windowW1, _windowW2, _windowH1, _windowH2;
    int _windowWidth, _windowHeight;
    uint _camWidth, _camHeight;
    int _sMin, _sMax, _sRmv;
    int _tMin, _tMax, _tRmv;
    int _mveRdrIdx; // mve render index
    int _S, _T; // LF angular range
    int _centralS, _centralT; // central view angular coordinates
    uint _nbCameras;
    uint _renderIndex;
    uint _centralIndex; // index of the central view (for optical flow)
    std::vector<InputCam*> _vCam;

    std::vector<std::vector<cv::Point2f> > _flowedLightField;
};

#endif /* #ifndef LF_SCENE_H */
