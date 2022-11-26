import './App.css'
import { useState } from 'react';
import Form from 'react-bootstrap/Form';
import { Button } from 'react-bootstrap';
import QRCode from 'react-qr-code';

const noop = () => { }
const colorConfig = [
  'gray',
  'red',
  'blue'
]
function Hold({ name, value, onClick = noop }) {

  if (value == undefined || value == null) {
    value = 0
  }
  // if (value !== 0)
  // console.log('Rendering', name, value, colorConfig[value])
  return (
    <div
      onClick={() => onClick(name, value)}
      className="col-sm-1"
      style={{
        cursor: 'pointer',
        backgroundColor: colorConfig[value],
        margin: '1px ',
        flexGrow: 1,
        color: '#6f6f6f'
      }}
    >
      {name}
    </div>
  )
}


function Wall({ numRow, numCol, boardCode, setBoardCode = noop }) {
  // console.log('render', holdStates)

  let rows = []
  let colNames = [
    '', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K'
  ]
  let cols = ['']
  for (let i = 0; i < numCol; i++) {
    cols.push(colNames[i + 1])
  }
  // add column label
  rows.push(
    <div style={{
      display: 'flex'
    }}>
      {cols.map((name) => <div key={name} style={{ flexGrow: 1 }}>{name}</div>)}
    </div>
  )

  const holdClickHandler = (holdCode, currentValue) => {
    const newValue = (currentValue + 1) % colorConfig.length
    const newState = { ...boardCode }
    newState[holdCode] = newValue
    console.log(holdCode, currentValue, newValue, colorConfig.length, newState)
    setBoardCode(newState)
  }

  for (let i = 0; i < numRow; i++) {
    let cols = []
    // add row label
    cols.push(
      <div
        key={`row${i}`}
        style={{
          margin: '1px',
          flexGrow: 1
        }}
      >
        {i + 1}
      </div>
    )
    // populate holds
    for (let j = 0; j < numCol; j++) {
      let holdCode = `${colNames[j + 1]}${i + 1}`
      cols.push(
        <Hold
          key={holdCode}
          name={holdCode}
          value={boardCode[holdCode]}
          onClick={holdClickHandler}
        />
      )
    }

    rows.push(
      <div style={{
        display: "flex",
        flexGrow: 1,
        key: i
      }}>
        {cols}
      </div>
    )
  }
  return (
    <div style={{
      height: '100%',
      display: "flex",
      flexDirection: "column-reverse"
    }}>
      {rows}
    </div>
  )
}



function QR({ value }) {
  return <QRCode
    size={256}
    style={{ height: "auto", maxWidth: "100%", width: "100%" }}
    value={value}
    viewBox={`0 0 256 256`}
  />
}

const indexMappingFunction = {
  standard: (code, numRow, numCol) => {
    console.log('code', code)
    const colCode = code[0]
    const rowCode = code[1]
    // const numRow = 8;
    const colMap = {
      A: 0, B: 1, C: 2, D: 3, E: 4, F: 5, G: 6, H: 7, I: 8, J: 9, K: 10, L: 11
    }

    const row = Number(rowCode)
    const col = colMap[colCode]
    let index;
    if (col % 2 == 0) {
      index = col * numRow + row;
    } else {
      index = col * numRow + (numRow - row);
    }
    return index
  }
}

function boardCodeToIndex(boardCode, numRow, numCol) {
  const codes = []
  const indexes = []
  Object.keys(boardCode).forEach((key) => {
    // let code = boardCode[key]
    codes.push(key)
    const index = indexMappingFunction.standard(key, numRow, numCol)
    indexes.push(index)
  })
  return indexes.join(' ')
}

export default function App() {
  const [showQR, setShowQR] = useState(false);
  const [boardCode, setBoardCode] = useState({})
  const [numRow, setNumRow] = useState(8)
  const [numCol, setNumCol] = useState(6)
  const value = boardCodeToIndex(boardCode, numRow, numCol);
  console.log(value)
  return (
    <main>

      <pre>{JSON.stringify(boardCode)}</pre>
      {!showQR ?
        <Wall
          numRow={numRow}
          numCol={numCol}
          boardCode={boardCode}
          setBoardCode={setBoardCode} /> :
        <QR value />}

      <Button
        onClick={() => setShowQR(!showQR)}
      >{showQR ? 'hide' : 'show'} QR</Button>
      <Button
        onClick={() => { setBoardCode({}) }}
      >Reset</Button>
      <Form>
        <Form.Group className="mb-3" controlId="formBasicEmail">
          <Form.Label>Num Row</Form.Label>
          <Form.Control type="number" placeholder="18" value={numRow}
            onChange={(e) => setNumRow(e.target.value)}
          />
          <Form.Label>Num Col</Form.Label>
          <Form.Control type="number" placeholder="11" value={numCol}
            onChange={(e) => setNumCol(e.target.value)}
          />
        </Form.Group>
      </Form>
      <QRCode
        size={256}
        style={{ height: "auto", maxWidth: "100%", width: "30%" }}
        value={value}
        viewBox={`0 0 256 256`}
      />
    </main>
  )
}