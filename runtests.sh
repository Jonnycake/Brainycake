make lib_test
echo -e "\n\nThis is the registry test..."
./registry_test
read n

echo -e "\n\nThis is the stack test..."
./stack_test
read n

echo "Making binary..."
make bin
echo -e "\n\nGoing to run the test brainycake scripts..."
for f in tests/*
do
    echo -e "\n\nProcessing $f...`date`"
    ./bcinterpreter $f
    echo -e "\nEnd Time: `date`"
    read n
done
make clean_test
