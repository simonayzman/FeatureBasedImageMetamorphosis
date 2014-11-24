#Feature Based Image Metamorphosis

by Peng Hui How, Josh Israel, and Simon Ayzman

See the final animations in the videos and animations directories, or on YouTube at: https://www.youtube.com/watch?v=GdLh_sE-e-o&list=PLjIR0u8ThmEVRRVTpQRo4wDKnG87FPNdU

See the presentation directory for a set of slides that contains a helpful overview of this project.

This implementation was based on the paper **_Feature-Based Image Metamorphosis_** by Thaddeus Beier and Shawn Neely.

###featureAdder

The featureAdder program is a robust interactive image processing program that uses OpenGL to allow users to specify common features (e.g., for faces: placement of eyes, nose, cheekbones, hair, etc.) in both the source and destination images. Specifically, the featureAdder program takes in the source and destination image (.png) files and outputs their respective feature (.feat) files. This output is a simple text format containing coordinates for each line segment, which can then be read in by the image morphing program (imageMorph). In some sense, featureAdder is an important bottleneck in the production of morphed images/videos. The ne can be run automatically without the user’s control. However, featureAdder needs to be a user-friendly and productive enough tool that makes the process of adding common feature simple and constructive; the users’ needs and patterns need to be taken into account. For example, the program is capable of undoing mistakes and/or improperly placed features in the order that they were added and as far back as necessary. Also, featureAdder is capable of opening/editing previously made feature files, removing the need to recreate a feature file from scratch every time for the same image. The undo feature is retained even for newly opened files, but the order in which the features were added is not preserved; the program simply alternates between undoing source and destination image features. Lastly, featureAdder is capable of taking screenshots of the current work environment. Once the features are written into .feat files, they will then be fed into and parsed by the imageMorph program introduced in the next section.

After you run the program, click anywhere to start drawing features. Keep in mind that the order that the features are drawn for each respective image will be maintained for the output phase; e.g., the sixth feature drawn on the source image (SI) will correlate to the sixth feature drawn on the destination image (DI), even if they were not meant to be correlated. That being said, the user can draw any number of times on either image without restriction. Five features can be drawn on the SI, followed by three on the DI, followed by two on the SI, followed by four on the DI. It is up to the user to keep track of the correct correlation of the features.

#####USAGE

**_Command line usage:_**
```
./featureAdder (.png source image filename) 
			   (.png destination image filename)
               [--s  (.png screenshot images base-name)]      		   
               [--es (input .feat for source image )]
			   [--ed (input .feat for destination image)]
			   [--os (output .feat base filename for source image)]
			   [--od (output .feat base filename for destination image)]
```

Command line arguments in brackets are optional.<br>
The s flag is for base-naming the screenshot files.<br>
The es/ed flags are for opening and editing feature files.<br>
The os/od flags are for base-naming the output feature files.<br>

**_Program usage:_**

Quit the program by pressing the 'q' key.<br>
Undo your previous action(s) by pressing the 'u' key.<br>
Produce a screenshot of your current window by pressing the 's' key. %<br>
Produce two .feat output files for your features by pressing the 'o' key. %<br>
Force-produce two .feat output files for your features by pressing the 'f' key. % ^<br>

% You may do this multiple times in one session. Multiple versions will be created.<br>
^ Required when number of features specified on the SI and DI not match.<br>

###imageMorph

This program generates the intermediate frames between the two images. The number of frames generated and the steepness of the change progression is up to the user. There are two modes available for now: a linear line (constant rate of change) and a truncated sine curve (changes in the beginning and at the end are gradual, but changes in the center are steep). At the moment, this program only supports .png image files; this is for simplicity, since pre-processing is easy.

#####USAGE

```
./imageMorph (.feat for source image) 
			 (.feat for destination image) 
             (.png for source image) 
             (.png for destination image) 
             (.png base filename for intermediary images) 
             (number of frames to generate) 
             (easing function)
```

The easing function can be either linear or sine, indicated by '-l' and '-s' respectively.

###combiner

This program combines a set of .png files that have a common base filename into a .avi video file.

#####USAGE

```
./combiner (.png base filename of all images) 
		   (total number of images in set)
           [.avi output video filename] 
           [number of frames per second]
```
Command line arguments in brackets are optional.<br>
By default, the output video filename is *output.avi* and the number of frames per second is 20.0.

###frameExtractor

This program separates a .avi video file into a set of .png image files.

#####USAGE

```
./frameExtractor (.avi input video) 
				 (number of frames to extract)
                 (.png output base filename)
```

###autoFeatures

This program recognizes facial features and attempts to create the respective .feat file that correlates to them. Currently, it is not very effective.

#####USAGE

```
./autoFeatures (.png input image) 
			   (.feat output base filename)
```

### Final Animation

In order to produce the final image, a lot of work went into the actual production of the intermediate images necessary. Doing it will take very many animator-hours. Using *frameExtractor*, the two original videos were broken down into frames. Then, specific keyframes were carefully selected. The *featureAdder* tool was then used on each respective pair of keyframes to specify common features. Two .feat files were made for each image pair. Then, *imageMorph* was run on all the images and their respective .feat files. The frames in each morph were then carefully selected to be added to the final set of animation frames. Finally, using *combiner*, these frames were all combined into a single video.


