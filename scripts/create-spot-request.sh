aws ec2 run-instances \
    --launch-template LaunchTemplateName=m5zn_metal_default,Version=10 \
    --count 1 \
    --instance-market-options file://spot-options.json
