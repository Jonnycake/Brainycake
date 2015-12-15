echo -e "\n\nThis is the registry test..."
./registry_test
read n

echo -e "\n\nThis is the stack test..."
./stack_test
read n

if [ $# -eq 1 ]
then
    if [ $1 == "bin" ]
    then
        echo "Making binary..."
        echo -e "\n\nGoing to run the test brainycake scripts..."
        for f in tests/*
        do
            echo -e "\n\nProcessing $f...`date`"
            ./bcinterpreter $f
            echo -e "\nEnd Time: `date`"
            read n
        done
    fi
fi
