mkdir .build
cd .build
cmake ../src/
cmake --build .
cmake ../src/ -DBACKUP=ON
cmake --build .
mv my_backup/my_backup ..
mv my_restore/my_restore ..
cd ..
rm -rf .build