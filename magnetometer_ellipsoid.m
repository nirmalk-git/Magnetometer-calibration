clc; clear all; close all;

data = csvread('C:\Users\nirmal\Desktop\magnetometer_calib.csv',1,0);
mag = data(:,2);

for i = 1:length(mag)/4
    magx(i) = mag(4*i-2);
    magy(i) = mag(4*i-1);
    magz(i) = mag(4*i);
end

% do the fitting
[ center, radii, evecs, v, chi2 ] = ellipsoid_fit( [magx' magy' magz'], '');
fprintf( 'Ellipsoid center: %.5g %.5g %.5g\n', center );
fprintf( 'Ellipsoid radii: %.5g %.5g %.5g\n', radii );
fprintf( 'Ellipsoid evecs:\n' );
fprintf( '%.5g %.5g %.5g\n%.5g %.5g %.5g\n%.5g %.5g %.5g\n', ...
    evecs(1), evecs(2), evecs(3), evecs(4), evecs(5), evecs(6), evecs(7), evecs(8), evecs(9) );
fprintf( 'Algebraic form:\n' );
fprintf( '%.5g ', v );
fprintf( '\nAverage deviation of the fit: %.5f\n', sqrt( chi2 / size( magx', 1 ) ) );
fprintf( '\n' );

% draw data
figure,
plot3( magx, magy, magz, 'bo' );
hold on;

%draw fit
mind = min( [ magx' magy' magz' ] );
maxd = max( [ magx' magy' magz' ] );
nsteps = 50;
step = ( maxd - mind ) / nsteps;
[ x, y, z ] = meshgrid( linspace( mind(1) - step(1), maxd(1) + step(1), nsteps ), linspace( mind(2) - step(2), maxd(2) + step(2), nsteps ), linspace( mind(3) - step(3), maxd(3) + step(3), nsteps ) );

Ellipsoid = v(1) *x.*x +   v(2) * y.*y + v(3) * z.*z + ...
          2*v(4) *x.*y + 2*v(5)*x.*z + 2*v(6) * y.*z + ...
          2*v(7) *x    + 2*v(8)*y    + 2*v(9) * z;
p = patch( isosurface( x, y, z, Ellipsoid, -v(10) ) );
hold off;
set( p, 'FaceColor', 'red', 'EdgeColor', 'none' );
view( -70, 40 );
axis vis3d equal;
camlight;
lighting phong;
