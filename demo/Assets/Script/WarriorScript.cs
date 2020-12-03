using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WarriorScript : MonoBehaviour
{
    public float moveSpeed;
    public float rotationSpeed;
    CharacterController cc;
    Rigidbody b;
    // Start is called before the first frame update
    void Start() {
        b = GetComponent<Rigidbody>();
        cc = GetComponent<CharacterController>();
        b.useGravity = false;
    }

    // Update is called once per frame
    void Update() {
        //float h = Input.GetAxis("Horizontal");
        //float v = Input.GetAxis("Vertical");
        //Vector3 dir = new Vector3(h, 0, v);
        //if (dir != Vector3.zero)
        //{
        //    float angle = Mathf.Atan2(dir.x, dir.z) * Mathf.Rad2Deg;
        //    if (90.0f > angle && angle > -90.0f)
        //    {
        //        angle = angle * rotationSpeed * Time.deltaTime;
        //        transform.Rotate(Vector3.up, angle);
        //    }
        //}
        //transform.Translate(dir * moveSpeed * Time.deltaTime);
        if (Input.GetKey(KeyCode.W))
        {
            cc.Move(this.transform.rotation * Vector3.forward * moveSpeed * Time.deltaTime);
        }
        if (Input.GetKey(KeyCode.A))
        {
            transform.Rotate(Vector3.up * -rotationSpeed * Time.deltaTime);
        }
        if (Input.GetKey(KeyCode.D))
        {
            transform.Rotate(Vector3.up * rotationSpeed * Time.deltaTime);
        }
    }
}
