package com.example.martilloveras.converter;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    TextView valor, result;
    RadioButton km, miles;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        valor = (TextView)findViewById(R.id.txtValor);
        result = (TextView)findViewById(R.id.txtResult);
        km = (RadioButton)findViewById(R.id.rdKm);
        miles = (RadioButton)findViewById(R.id.rdMiles);
        setContentView(R.layout.activity_main);
    }

    public void convert(View view){
        float val = Float.parseFloat(valor.getText().toString());
        double resultat;
        if (km.isChecked()){
            resultat = val / 1.609344;
        }
        else{
            resultat = val * 1.609344;
        }
        //result.setText(""+resultat);
    }
}
