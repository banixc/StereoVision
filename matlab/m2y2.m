function m2y2()
% 加载标定的结果文件 %
load('matlab.mat');

% 读取并进行参数预处理 %
cameraMatrix1 = stereoParams.CameraParameters1.IntrinsicMatrix';
cameraMatrix2 = stereoParams.CameraParameters2.IntrinsicMatrix';
distCoeffs1 = [stereoParams.CameraParameters1.RadialDistortion,stereoParams.CameraParameters1.TangentialDistortion,0]';
distCoeffs2 = [stereoParams.CameraParameters2.RadialDistortion,stereoParams.CameraParameters2.TangentialDistortion,0]';
R = stereoParams.RotationOfCamera2';
T = stereoParams.TranslationOfCamera2';

% 将matlab文件转换为opencv能识别的格式并写入到文件里 %
filename = 'stereo.yml';
dym_matlab2opencv( cameraMatrix1, filename);
dym_matlab2opencv( cameraMatrix2, filename, 'a');
dym_matlab2opencv( distCoeffs1, filename, 'a');
dym_matlab2opencv( distCoeffs2, filename, 'a');
dym_matlab2opencv( R, filename, 'a');
dym_matlab2opencv( T, filename, 'a');