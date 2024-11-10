rm -rf .build
mkdir .build
cd .build
cmake ../src/
cmake --build .
mv my_backup/my_backup ..
mv my_restore/my_restore ..
cd ..
rm -rf .build