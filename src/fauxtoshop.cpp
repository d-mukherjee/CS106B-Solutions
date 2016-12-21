// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own.
// This program details a photo editor that has the features of-
// 1. Scatter 2. Edge Detection 3. Green Screen Overlay 4. Image Comparison

#include <iostream>
#include "console.h"
#include "gwindow.h"
#include "grid.h"
#include "simpio.h"
#include "strlib.h"
#include "gbufferedimage.h"
#include "gevents.h"
#include "math.h" //for sqrt and exp in the optional Gaussian kernel
using namespace std;

static const int    WHITE = 0xFFFFFF;
static const int    BLACK = 0x000000;
static const int    GREEN = 0x00FF00;
static const double PI    = 3.14159265;

void     doFauxtoshop(GWindow &gw, GBufferedImage &img);

bool     openImageFromFilename(GBufferedImage& img, string filename);
bool 	saveImageToFilename(const GBufferedImage &img, string filename);
void     getMouseClickLocation(int &row, int &col);

/* STARTER CODE FUNCTION - DO NOT EDIT
 *
 * This main simply declares a GWindow and a GBufferedImage for use
 * throughout the program. By asking you not to edit this function,
 * we are enforcing that the GWindow have a lifespan that spans the
 * entire duration of execution (trying to have more than one GWindow,
 * and/or GWindow(s) that go in and out of scope, can cause program
 * crashes).
 */
int main() {
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setVisible(true);
    GBufferedImage img;
    doFauxtoshop(gw, img);
    return 0;
}

//All the methods used in the program

void doFauxtoshop(GWindow &gw, GBufferedImage &img);
bool openImage(GWindow &gw, GBufferedImage &img);
void getUserChoice(int &option);
void saveImageOrClose(GWindow &gw, GBufferedImage &img);
void scatter(GBufferedImage &img);
int calculateNewR(int r, int degree);
int calculateNewC(int c, int degree);
void edgeDetection(GBufferedImage &img);
int getDistance(int &pixels1, int &pixels2);
void neighboursCheck (Grid<int> &pixels, int &r, int &c, int &threshold, bool &thresholdCheck);
void greenScreen(GBufferedImage &img1);
Grid<int> getOverlayImage();
void getToleranceAndLocation (int &tolerance, int &row, int &col);
void compareImages(GBufferedImage &img1);

/**
 * @brief doFauxtoshop The console of the program that the user interacts with.
 * Offers the four options that the user can choose from- 1. Scatter 2. Edge Detect 3. Green Screen
 * 4. Compare Image
 * @param gw The Graphics Window
 * @param img The Buffered Image
 */

void doFauxtoshop(GWindow &gw, GBufferedImage &img) {
    cout<<"Welcome to Fauxtoshop!"<<endl;
    int option;
    while(true)
    {
        bool result = openImage(gw, img); //Opens the image and stores it in a grid
        while (!result) {
            cout<<"Invalid file name."<<endl;
            gw.clear();
            result = openImage(gw, img);
        }
        getUserChoice(option); //Notes the user's choice
        if (option == 1) {
            scatter(img); //Calls the function that performs the Scatter option
            saveImageOrClose(gw, img);
        }
        else if (option == 2) {
             edgeDetection(img); //Calls the function that performs the Edge Detect option
             saveImageOrClose(gw, img);
        }
        else if (option == 3) {
              greenScreen(img); //Calls the function that performs the Green Screen option
              saveImageOrClose(gw, img);
        }
        else if (option == 4) {
              compareImages(img); //Calls the function that compares the images
              saveImageOrClose(gw, img);
        }
        else {
              cout <<"Please enter a number between 1 and 4"<<endl; //Error message if there is an invlid entry
        }
    }
}

/**
 * @brief openImage Opens Image from the file and displays it on the canvas.
 * @param gw The Graphic Window
 * @param img The Buffered Image
 */

bool openImage(GWindow &gw, GBufferedImage &img) {
    string prompt = "Enter name of Image file to open (or blank to quit): ";
    string fileName = getLine(prompt);
    cout <<"Opening image file, may take a minute..."<<endl;
    bool result = openImageFromFilename(img, fileName);
    gw.requestFocus();
    gw.add(&img,0,0);
    gw.setCanvasSize(img.getWidth(), img.getHeight());
    return result;
}

/**
 * @brief getUserChoice Obtains the choice from the user
 * @param option The integer that hold the choice of the user
 */
void getUserChoice(int &option) {
    cout <<"Which image filter would you like to apply?"<<endl;
    cout<<"     1 - Scatter"<<endl;
    cout<<"     2 - Edge Detection"<<endl;
    cout<<"     3 - \"Green Screen\" with another image"<<endl;
    cout<<"     4 - Compare image with another image"<<endl;
    option = getInteger("Your choice: ");
}

/**
 * @brief saveImageOrClose Offers the user the options to either save the image or close the window.
 * @param gw The Graphics Window
 * @param img The Buffered Image
 */
void saveImageOrClose(GWindow &gw, GBufferedImage &img) {
    string prompt = "Enter filename to save image or blank to skip saving";
    string fileToSave = getLine(prompt);
    if (fileToSave.length() > 0) {
        saveImageToFilename(img, fileToSave);
    }
    else {
        gw.clear();
    }
}

/**
 * @brief scatter Scatters the pixels in the image to give a distorted image.
 * @param img The Buffered Image that is worked on
 */
void scatter(GBufferedImage &img){
    int degree;
    string prompt = "Enter degree of scatter (1-100): ";
    degree = getInteger(prompt);
    while (degree > 100) {
        string reprompt = "Invalid. Enter a value of a degree between 1 and 100";
        degree = getInteger(reprompt);
    }
    Grid<int> pixels = img.toGrid();
    Grid<int> newGrid (pixels.numRows(), pixels.numCols());

    for (int r = 0; r < pixels.numRows(); r++) {
        for (int c = 0; c < pixels.numCols(); c++) {
            int newR = calculateNewR(r, degree);
            int newC = calculateNewC(c, degree);
            while (newGrid.inBounds(newR, newC) != true) {
                newR = calculateNewR(r, degree);
                newC = calculateNewC(c, degree);
            }
            newGrid[r][c] = pixels[newR][newC];
        }
    }
    img.fromGrid(newGrid);
}

/**
 * @brief calculateNewR Calculates the new row after displacement
 * @param r Previous row number
 * @param degree Displacement measure
 * @return The new row number
 */
int calculateNewR(int r, int degree) {
   int newR = randomInteger(r - degree, r + degree);
   return newR;
}

/**
 * @brief calculateNewC Calculates the new column after displacement
 * @param c Previous column number value
 * @param degree Displacement meqsure
 * @return The new coumn number
 */
int calculateNewC(int c, int degree) {
   int newC = randomInteger(c - degree, c + degree);
   return newC;
}

/**
 * @brief edgeDetection Performs a fucntion on the image that outlines all the edges
 * @param img The Buffered Image that has to be worked upon
 */

void edgeDetection(GBufferedImage &img) {
    string prompt = "Enter threshold for edge detection";
    int threshold = getInteger(prompt);
    while (threshold < 0) {
        threshold = 0;
        string reprompt = "Invalid. The value of threshold must be positive";
        threshold = getInteger(reprompt);
    }
    Grid<int> pixels = img.toGrid();
    Grid<int> newGrid (pixels.numRows(), pixels.numCols());
    bool thresholdCheck;
    for(int r = 0; r < pixels.numRows(); r++) {
        for(int c = 0; c < pixels.numCols(); c++) {
            neighboursCheck(pixels, r, c, threshold, thresholdCheck);
            if (thresholdCheck == true) {
                newGrid[r][c] = BLACK;
            }
            else {
                newGrid[r][c] = WHITE;
            }
        }
    }
    img.fromGrid(newGrid);
}

/**
 * @brief getDistance Calculates the Euclidian distance between two colors.
 * @param pixels1 First color
 * @param pixels2 Second color
 * @return Distance between the two colors
 */
int getDistance(int &pixels1, int &pixels2) {
    int r1, g1, b1, r2, g2, b2;
    GBufferedImage::getRedGreenBlue(pixels1, r1, g1, b1);
    GBufferedImage::getRedGreenBlue(pixels2, r2, g2, b2);
    int dRed = abs(r2 - r1);
    int dBlue = abs(b2 - b1);
    int dGreen = abs(g2 - g1);
    return max(dRed, max(dBlue, dGreen)); //Citing source- Modified this after help from the LaIR

}

/**
 * @brief neighboursCheck Checks the difference of the pixel value with all of its neighbors
 * @param pixels
 * @param r Row number
 * @param c Column number
 * @param threshold The threshold value entered by the user
 * @param thresholdCheck The Boolean counter that keeps track of the edges
 */
void neighboursCheck (Grid<int> &pixels, int &r, int &c, int &threshold, bool &thresholdCheck) {
    int maxdifference = -1;
    for (int drow = -1; drow <=1; drow++) {
        for (int dcol = -1; dcol <=1; dcol++) {
            if(pixels.inBounds(r+drow, c+dcol)) {
                int difference = getDistance(pixels[r][c], pixels[r+drow][c+dcol]);
                if (difference > maxdifference) {
                    maxdifference = difference;
                }
            }
        }
    }
    if (maxdifference > threshold) {
        thresholdCheck = true;
    }
    else {
        thresholdCheck = false;
    }
}

/**
 * @brief greenScreen Performs the green screen overlay function on the image. (Even after multiple attempts, this function
 * does not work completely.)
 * @param img1 The Buffered Image to be worked upon
 */
void greenScreen(GBufferedImage &img){
    Grid<int>pixels1 = img.toGrid();
    Grid<int>pixels2 = getOverlayImage();\
    int tolerance, row, col;
    getToleranceAndLocation(tolerance, row, col);
    cout<<"You chose ("<<row<<","<<col<<")"<<endl;

    int color = GREEN;
    for (int r = 0; r < pixels2.numRows(); r++) {
        for (int c = 0; c < pixels2.numCols(); c++) {
            int greenDifference = getDistance(pixels2[r][c], color);
            if (pixels1.inBounds(row, col)) { //Citing source- Added this line of code after help from the LaIR
                if (greenDifference > tolerance) {
                    pixels1[row][col] = pixels2[r][c];
                }
            }
            col++;
        }
        row++;
    }
    img.fromGrid(pixels1);
}

/**
 * @brief getOverlayImage Get the image from the user that has to be layed over the image
 * @param pixels
 */
 Grid<int> getOverlayImage() {
    cout<<"Now choose another image to add to your background image."<<endl;
    string prompt1 = "Enter name of image file to open: ";
    string fileName = getLine(prompt1);
    GBufferedImage img;
    cout<<"Opening image file, may take a minute..."<<endl;
    openImageFromFilename(img, fileName);
    return img.toGrid();
}

/**
 * @brief getToleranceAndLocation Gets the values of the tolerance allowed and the location where the image has to be put
 * @param tolerance The degree of tolerance allowed
 * @param row Row number of the position selected
 * @param col Column number of the position selected
 */
void getToleranceAndLocation (int &tolerance, int &row, int &col) {
    string prompt2 = "Now choose a tolerance threshold: ";
    tolerance = getInteger(prompt2);
    string prompt3 = "Enter location to place image as \"(row,col)\" (or blank to use mouse): ";
    string location = getLine(prompt3);
    if (location.length() < 1) {
      getMouseClickLocation(row, col);
    }
    else {
       int commaLocation = stringIndexOf(location, ",");
       row = stringToInteger(location.substr(1, commaLocation-1));
       col = stringToInteger(location.substr(commaLocation + 1, location.length() - commaLocation - 2));
    }
}

/**
 * @brief compareImages Compares two images and find the number of pixels that differ
 * @param img1
 */
void compareImages(GBufferedImage &img1){
    cout<<"Now choose another image file to compare to"<<endl;
    string prompt = "Enter image file to be opened: ";
    string fileName = getLine(prompt);
    GBufferedImage img2;
    cout<<"Opening image file, may take a minute..."<<endl;
    openImageFromFilename(img2, fileName);
    int diffPixels = img1.countDiffPixels(img2);
    if (diffPixels > 0) {
        cout<<"These images differ in "<<diffPixels<<" pixel locations!"<<endl;
    }
    else {
        cout<<"These images are the same!"<<endl;
    }

}


/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to open the image file 'filename'.
 *
 * This function returns true when the image file was successfully
 * opened and the 'img' object now contains that image, otherwise it
 * returns false.
 */
bool openImageFromFilename(GBufferedImage& img, string filename) {
    try { img.load(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Attempts to save the image file to 'filename'.
 *
 * This function returns true when the image was successfully saved
 * to the file specified, otherwise it returns false.
 */
bool saveImageToFilename(const GBufferedImage &img, string filename) {
    try { img.save(filename); }
    catch (...) { return false; }
    return true;
}

/* STARTER CODE HELPER FUNCTION - DO NOT EDIT
 *
 * Waits for a mouse click in the GWindow and reports click location.
 *
 * When this function returns, row and col are set to the row and
 * column where a mouse click was detected.
 */
void getMouseClickLocation(int &row, int &col) {
    GMouseEvent me;
    do {
        me = getNextEvent(MOUSE_EVENT);
    } while (me.getEventType() != MOUSE_CLICKED);
    row = me.getY();
    col = me.getX();
}

/* OPTIONAL HELPER FUNCTION
 *
 * This is only here in in case you decide to impelment a Gaussian
 * blur as an OPTIONAL extension (see the suggested extensions part
 * of the spec handout).
 *
 * Takes a radius and computes a 1-dimensional Gaussian blur kernel
 * with that radius. The 1-dimensional kernel can be applied to a
 * 2-dimensional image in two separate passes: first pass goes over
 * each row and does the horizontal convolutions, second pass goes
 * over each column and does the vertical convolutions. This is more
 * efficient than creating a 2-dimensional kernel and applying it in
 * one convolution pass.
 *
 * This code is based on the C# code posted by Stack Overflow user
 * "Cecil has a name" at this link:
 * http://stackoverflow.com/questions/1696113/how-do-i-gaussian-blur-an-image-without-using-any-in-built-gaussian-functions
 *
 */
Vector<double> gaussKernelForRadius(int radius) {
    if (radius < 1) {
        Vector<double> empty;
        return empty;
    }
    Vector<double> kernel(radius * 2 + 1);
    double magic1 = 1.0 / (2.0 * radius * radius);
    double magic2 = 1.0 / (sqrt(2.0 * PI) * radius);
    int r = -radius;
    double div = 0.0;
    for (int i = 0; i < kernel.size(); i++) {
        double x = r * r;
        kernel[i] = magic2 * exp(-x * magic1);
        r++;
        div += kernel[i];
    }
    for (int i = 0; i < kernel.size(); i++) {
        kernel[i] /= div;
    }
    return kernel;
}
