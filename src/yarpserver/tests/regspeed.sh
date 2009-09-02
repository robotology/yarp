
let x=1
let top=200
while [ $x -le $top ]; do
    echo $x of $top
    echo Hello | yarp write /write
    let x=x+1
done

